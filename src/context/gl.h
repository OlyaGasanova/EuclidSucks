#ifndef H_CONTEXT_GL
#define H_CONTEXT_GL

#include "context.h"
#include <gl/GL.h>
#include <gl/glext.h>
#include <gl/wglext.h>

// Buffers
    PFNGLGENBUFFERSARBPROC              glGenBuffers;
    PFNGLDELETEBUFFERSARBPROC           glDeleteBuffers;
    PFNGLBINDBUFFERARBPROC              glBindBuffer;
    PFNGLBUFFERDATAARBPROC              glBufferData;
    PFNGLGENVERTEXARRAYSPROC            glGenVertexArrays;
    PFNGLDELETEVERTEXARRAYSPROC         glDeleteVertexArrays;
    PFNGLBINDVERTEXARRAYPROC            glBindVertexArray;
// Textures
    PFNGLACTIVETEXTUREPROC              glActiveTexture;
    PFNGLGENERATEMIPMAPPROC             glGenerateMipmap;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC       glCompressedTexImage2D;
// Shader
    PFNGLCREATEPROGRAMPROC              glCreateProgram;
    PFNGLDELETEPROGRAMPROC              glDeleteProgram;
    PFNGLLINKPROGRAMPROC                glLinkProgram;
    PFNGLUSEPROGRAMPROC                 glUseProgram;
    PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog;
    PFNGLCREATESHADERPROC               glCreateShader;
    PFNGLDELETESHADERPROC               glDeleteShader;
    PFNGLSHADERSOURCEPROC               glShaderSource;
    PFNGLATTACHSHADERPROC               glAttachShader;
    PFNGLCOMPILESHADERPROC              glCompileShader;
    PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog;
    PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation;
    PFNGLUNIFORM1IVPROC                 glUniform1iv;
    PFNGLUNIFORM1FVPROC                 glUniform1fv;
    PFNGLUNIFORM2FVPROC                 glUniform2fv;
    PFNGLUNIFORM3FVPROC                 glUniform3fv;
    PFNGLUNIFORM4FVPROC                 glUniform4fv;
    PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv;
    PFNGLBINDATTRIBLOCATIONPROC         glBindAttribLocation;
    PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray;
    PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer;
    PFNGLGETPROGRAMIVPROC               glGetProgramiv;
// Frame Buffers
    PFNGLGENFRAMEBUFFERSPROC            glGenFramebuffers;
    PFNGLBINDFRAMEBUFFERPROC            glBindFramebuffer;
    PFNGLGENRENDERBUFFERSPROC           glGenRenderbuffers;
    PFNGLBINDRENDERBUFFERPROC           glBindRenderbuffer;
    PFNGLFRAMEBUFFERTEXTURE2DPROC       glFramebufferTexture2D;
    PFNGLFRAMEBUFFERRENDERBUFFERPROC    glFramebufferRenderbuffer;
    PFNGLRENDERBUFFERSTORAGEPROC        glRenderbufferStorage;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC     glCheckFramebufferStatus;
    PFNGLDELETEFRAMEBUFFERSPROC         glDeleteFramebuffers;
    PFNGLDELETERENDERBUFFERSPROC        glDeleteRenderbuffers;
// Common
    PFNGLSTENCILFUNCSEPARATEPROC        glStencilFuncSeparate;
    PFNGLSTENCILMASKSEPARATEPROC        glStencilMaskSeparate;
    PFNGLSTENCILOPSEPARATEPROC          glStencilOpSeparate;
    PFNGLBLENDFUNCSEPARATEPROC          glBlendFuncSeparate;

#define GetProcOGL(x) x=(decltype(x))wglGetProcAddress(#x);


struct BufferGL : Buffer {
    GLuint id;
    GLenum target; 

    BufferGL(const Buffer::Desc &desc) : Buffer(desc) {
        if (desc.flags & BUF_INDEX) {
            target = GL_ELEMENT_ARRAY_BUFFER;
        }

        if (desc.flags & BUF_VERTEX) {
            target = GL_ARRAY_BUFFER;
        }

        glBindVertexArray(0);
        glGenBuffers(1, &id);
        glBindBuffer(target, id);
        glBufferData(target, desc.count * desc.stride, desc.data, (desc.flags & BUF_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    }

    virtual ~BufferGL() {
        glDeleteBuffers(1, &id);
    }
};

struct TextureGL : Texture {
    GLuint id;
    GLenum target;

    TextureGL(const Texture::Desc &desc) : Texture(desc) {
        const static struct FormatInfo {
            GLenum iformat, format, type, bpp;
        } info[] = {
            { 0,                                           0,              0,                                       1 }, // FMT_UNKNOWN
            { GL_R8,                                       GL_RED,         GL_UNSIGNED_BYTE,                        8 }, // FMT_R8
            { GL_RG8,                                      GL_RG,          GL_UNSIGNED_BYTE,                       16 }, // FMT_RG8
            { GL_RGBA8,                                    GL_RGBA,        GL_UNSIGNED_BYTE,                       32 }, // FMT_RGBA8
            { GL_SRGB8_ALPHA8_EXT,                         GL_RGBA,        GL_UNSIGNED_BYTE,                       32 }, // FMT_RGBA8_SRGB
            { GL_R16F,                                     GL_RED,         GL_HALF_FLOAT,                          16 }, // FMT_R16F
            { GL_RG16F,                                    GL_RG,          GL_HALF_FLOAT,                          32 }, // FMT_RG16F
            { GL_RGBA16F,                                  GL_RGBA,        GL_HALF_FLOAT,                          64 }, // FMT_RGBA16F
            { GL_R32F,                                     GL_RED,         GL_FLOAT,                               32 }, // FMT_R32F
            { GL_RG32F,                                    GL_RG,          GL_FLOAT,                               64 }, // FMT_RG32F
            { GL_RGB32F,                                   GL_RGB,         GL_FLOAT,                               96 }, // FMT_RGB32F
            { GL_RGBA32F,                                  GL_RGBA,        GL_FLOAT,                              128 }, // FMT_RGBA32F
            { GL_R11F_G11F_B10F_EXT,                       GL_RGB,         GL_UNSIGNED_INT_10F_11F_11F_REV_EXT,    32 }, // FMT_R11G11B10
            { GL_COMPRESSED_RGB_S3TC_DXT1_EXT,             0,              0,                                       4 }, // FMT_BC1
            { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,      0,              0,                                       4 }, // FMT_BC1_SRGB
            { GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            0,              0,                                       8 }, // FMT_BC2
            { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,      0,              0,                                       8 }, // FMT_BC2_SRGB
            { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            0,              0,                                       8 }, // FMT_BC3
            { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,      0,              0,                                       8 }, // FMT_BC3_SRGB
            { GL_COMPRESSED_RED_RGTC1,                     0,              0,                                       4 }, // FMT_BC4
            { GL_COMPRESSED_RG_RGTC2,                      0,              0,                                       8 }, // FMT_BC5
            { GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,       0,              0,                                       8 }, // FMT_BC6
            { GL_COMPRESSED_RGBA_BPTC_UNORM,               0,              0,                                       8 }, // FMT_BC7
            { GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,         0,              0,                                       8 }, // FMT_BC7_SRGB
        };

        ASSERT(COUNT(info) == Texture::FMT_MAX);
        ASSERT(desc.format != Texture::FMT_UNKNOWN);

        bool isCubemap = (desc.flags & FLAG_CUBEMAP);

        if (isCubemap) {
            target = GL_TEXTURE_CUBE_MAP;
        } else {
            target = GL_TEXTURE_2D;
        }

        glGenTextures(1, &id);
        glBindTexture(target, id);

        uint8 *ptr = (uint8*)desc.data;
        FormatInfo fmt = info[desc.format];

        int numFaces = isCubemap ? 6 : 1;

        for (int face = 0; face < numFaces; face++) {

            GLenum faceTarget = isCubemap ? (GL_TEXTURE_CUBE_MAP_POSITIVE_X + face) : target;
            uint32 w = desc.width;
            uint32 h = desc.height;

            for (int level = 0; level < desc.levels; level++) {
                int size = w * h * fmt.bpp / 8;
                if (desc.format >= FMT_BC1) {
                    size = max(size, int(4 * 4 * fmt.bpp / 8));
                    glCompressedTexImage2D(faceTarget, level, fmt.iformat, w, h, 0, size, ptr);
                } else {
                    glTexImage2D(faceTarget, level, fmt.iformat, w, h, 0, fmt.format, fmt.type, ptr);
                }
                ptr += size;
                w /= 2;
                h /= 2;
            }

        }

        bool genmips = (desc.flags & FLAG_GEN_MIPS);
        bool mipmaps = (desc.levels > 1) || genmips;
        bool nearest = (desc.flags & FLAG_NEAREST);
        bool repeat  = (desc.flags & FLAG_REPEAT);

        GLint wrap      = repeat  ? GL_REPEAT : GL_CLAMP_TO_EDGE;
        GLint filter    = nearest ? GL_NEAREST : GL_LINEAR;
        GLint filterMip = mipmaps ? (nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) : filter;

        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMip);

        if (genmips) {
            glGenerateMipmap(target);
        }
    }

    virtual ~TextureGL() {
        glDeleteTextures(1, &id);
    }

    void bind(ShaderSampler sampler) const {
        glActiveTexture(GL_TEXTURE0 + sampler);
        glBindTexture(target, id);
    }
};


struct ShaderGL : Shader {
    GLuint id;
    GLint  uid[uMAX];

    ShaderGL(const Shader::Desc &desc) : Shader(desc) {
        static const char *ShaderAttribName[aMAX]  = { SHADER_ATTRIBS(DECL_STR)  };
        static const char *ShaderSamplerName[sMAX] = { SHADER_SAMPLERS(DECL_STR) };
        static const char *ShaderUniformName[uMAX] = { SHADER_UNIFORMS(DECL_STR) };

        const char *VS_DEFINES = "#version 130\n"
                                 "#define VS\n"
                                 "#define varying   out\n"
                                 "#define attribute in\n"
                                 "#define texture2D texture\n";

        const char *FS_DEFINES = "#version 130\n"
                                 "#define FS\n"
                                 "#define varying     in\n"
                                 "#define texture2D   texture\n"
                                 "#define texture3D   texture\n"
                                 "#define textureCube texture\n"
                                 "out vec4 fragColor;\n";

        char *source = new char[desc.size + 1];
        memcpy(source, desc.data, desc.size);
        source[desc.size] = '\0'; // should be null-terminated

        const int SRC_COUNT = 3;
        const int type[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
        const char *code[2][SRC_COUNT] = {
                { VS_DEFINES, "#line 0\n", source },
                { FS_DEFINES, "#line 0\n", source }
            };

        GLchar info[1024];

        id = glCreateProgram();

        for (int i = 0; i < 2; i++) {
            GLuint obj = glCreateShader(type[i]);
            glShaderSource(obj, SRC_COUNT, code[i], NULL);
            glCompileShader(obj);

            glGetShaderInfoLog(obj, sizeof(info), NULL, info);
            if (info[0] && strlen(info) > 8) {
                LOG("! shader: %s\n", info);
            }

            glAttachShader(id, obj);
            glDeleteShader(obj);
        }

        delete[] source;

        // set vertex attribute indices (before linking)
        for (int at = 0; at < aMAX; at++) {
            glBindAttribLocation(id, at, ShaderAttribName[at]);
        }

        glLinkProgram(id);

        glGetProgramInfoLog(id, sizeof(info), NULL, info);
        if (info[0] && strlen(info) > 8) {
            LOG("! program: %s\n", info);
        }

        GLint success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        ASSERT(success != 0);

        glUseProgram(id);

        // set sampler indices
        for (int s = 0; s < sMAX; s++) {
            GLint idx = glGetUniformLocation(id, (GLchar*)ShaderSamplerName[s]);
            if (idx != -1) {
                glUniform1iv(idx, 1, &s);
            }
        }

        // get uniform indices
        for (int u = 0; u < uMAX; u++) {
            uid[u] = glGetUniformLocation(id, (GLchar*)ShaderUniformName[u]);
        }
    }

    virtual ~ShaderGL() {
        glDeleteProgram(id);
    }

    void bind() {
        glUseProgram(id);
    }
};


struct MeshGL : Mesh {
    GLuint id;

    MeshGL(const Mesh::Desc &desc) : Mesh(desc) {
        BufferGL *ibuf = (BufferGL*)desc.iBuffer;
        BufferGL *vbuf = (BufferGL*)desc.vBuffer;

        glGenVertexArrays(1, &id);
        glBindVertexArray(id);
        glBindBuffer(ibuf->target, ibuf->id);
        glBindBuffer(vbuf->target, vbuf->id);

        Vertex *v = (Vertex*)NULL + desc.vStart;
        glEnableVertexAttribArray(aPosition);
        glEnableVertexAttribArray(aNormal);
        glEnableVertexAttribArray(aTangent);
        glEnableVertexAttribArray(aTexCoord);
        glEnableVertexAttribArray(aColor);

        glVertexAttribPointer(aPosition, 3, GL_FLOAT, false, vbuf->desc.stride, &v->position);
        glVertexAttribPointer(aNormal,   3, GL_FLOAT, false, vbuf->desc.stride, &v->normal);
        glVertexAttribPointer(aTangent,  4, GL_FLOAT, false, vbuf->desc.stride, &v->tangent);
        glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, false, vbuf->desc.stride, &v->texcoord);
        glVertexAttribPointer(aColor,    4, GL_FLOAT, false, vbuf->desc.stride, &v->color);

        glBindVertexArray(0);
    }

    virtual ~MeshGL() {
        glDeleteVertexArrays(1, &id);
    }
};


struct FrameBufferGL : FrameBuffer {
    GLuint id;

    FrameBufferGL(const FrameBuffer::Desc &desc) : FrameBuffer(desc) {
        glGenFramebuffers(1, &id);
    }

    virtual ~FrameBufferGL() {
        glDeleteFramebuffers(1, &id);
    }

    void bind() {
        if (!this) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, id);
        // TODO
    }
};


static const GLenum BLEND_FUNC[] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_SRC1_COLOR,
    GL_ONE_MINUS_SRC1_COLOR,
    GL_SRC1_ALPHA,
    GL_ONE_MINUS_SRC1_ALPHA,
};

static const GLenum STENCIL_OP[] = {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_DECR,
};

static const GLenum COMPARE_FUNC[] = {
    GL_ALWAYS,
    GL_NEVER,
    GL_EQUAL,
    GL_NOTEQUAL,
    GL_LESS,
    GL_LEQUAL,
    GL_GREATER,
    GL_GEQUAL,
};

struct RenderStateGL : RenderState {
    RenderStateGL(const RenderState::Desc &desc) : RenderState(desc) {}

    void bind(const RenderState *curState, uint8 stencilRef) {
        #define DIFF(param) (!curState || desc.##param != curState->desc.##param)

        if DIFF(colorMask) {
            glColorMask((desc.colorMask & COLOR_MASK_R), 
                        (desc.colorMask & COLOR_MASK_G), 
                        (desc.colorMask & COLOR_MASK_B), 
                        (desc.colorMask & COLOR_MASK_A));
        }

        if DIFF(blending) {
            desc.blending ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        }

        if (desc.blending && (DIFF(blend.colorOpSrc) || DIFF(blend.colorOpDst) || DIFF(blend.alphaOpSrc) || DIFF(blend.alphaOpDst))) {
            glBlendFuncSeparate(BLEND_FUNC[desc.blend.colorOpSrc], BLEND_FUNC[desc.blend.colorOpDst],
                                BLEND_FUNC[desc.blend.alphaOpSrc], BLEND_FUNC[desc.blend.alphaOpDst]);
        }

        if DIFF(depthWrite) {
            glDepthMask(desc.depthWrite);
        }

        if DIFF(depthTest) {
            desc.depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        }

        if DIFF(stencilTest) {
            desc.stencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
        }

        if (desc.stencilTest) {
            for (int i = 0; i < FACE_MAX; i++) {
                const RenderState::Desc::Stencil &f = desc.stencil[i];

                GLenum face = (i == FACE_BACK) ? GL_BACK : GL_FRONT;

                if (DIFF(stencil[i].sfail) || DIFF(stencil[i].zfail) || DIFF(stencil[i].zpass)) {
                    glStencilOpSeparate(face, STENCIL_OP[f.sfail], STENCIL_OP[f.zfail], STENCIL_OP[f.zpass]);
                }

                if DIFF(stencil[i].writeMask) {
                    glStencilMaskSeparate(face, f.writeMask);
                }

                if (DIFF(stencil[i].compare) || DIFF(stencil[i].readMask)) {
                    glStencilFuncSeparate(face, COMPARE_FUNC[f.compare], stencilRef, f.readMask);
                }
            }
        }

        if DIFF(cullFace) {
            switch (desc.cullFace) {
                case FACE_FRONT : glCullFace(GL_FRONT);    break;
                case FACE_BACK  : glCullFace(GL_BACK);     break;
                case FACE_NONE  : glDisable(GL_CULL_FACE); return;
            }
            if (!curState || curState->desc.cullFace == FACE_NONE) {
                glEnable(GL_CULL_FACE);
            }
        }

        if (desc.shader && DIFF(shader)) {
            ShaderGL *obj = (ShaderGL*)desc.shader;
            obj->bind();
        }

        #undef DIFF
    }

    void setStencilRef(uint8 stencilRef) {
        const RenderState::Desc::Stencil &f = desc.stencil[FACE_FRONT];
        const RenderState::Desc::Stencil &b = desc.stencil[FACE_BACK];

        glStencilFuncSeparate(GL_FRONT, COMPARE_FUNC[f.compare], stencilRef, f.readMask);
        glStencilFuncSeparate(GL_BACK,  COMPARE_FUNC[b.compare], stencilRef, b.readMask);
    }
};


struct ContextGL : Context {
    RenderState *defRenderState;

    uint8 stencilRef;

    ContextGL() : Context(), stencilRef(0xFF) {
    // Buffers
        GetProcOGL( glGenBuffers );
        GetProcOGL( glDeleteBuffers );
        GetProcOGL( glBindBuffer );
        GetProcOGL( glBufferData );
        GetProcOGL( glGenVertexArrays );
        GetProcOGL( glDeleteVertexArrays );
        GetProcOGL( glBindVertexArray );
    // Textures
        GetProcOGL( glActiveTexture );
        GetProcOGL( glGenerateMipmap );
        GetProcOGL( glCompressedTexImage2D );
    // Shader
        GetProcOGL( glCreateProgram );
        GetProcOGL( glDeleteProgram );
        GetProcOGL( glLinkProgram );
        GetProcOGL( glUseProgram );
        GetProcOGL( glGetProgramInfoLog );
        GetProcOGL( glCreateShader );
        GetProcOGL( glDeleteShader );
        GetProcOGL( glShaderSource );
        GetProcOGL( glAttachShader );
        GetProcOGL( glCompileShader );
        GetProcOGL( glGetShaderInfoLog );
        GetProcOGL( glGetUniformLocation );
        GetProcOGL( glUniform1iv );
        GetProcOGL( glUniform1fv );
        GetProcOGL( glUniform2fv );
        GetProcOGL( glUniform3fv );
        GetProcOGL( glUniform4fv );
        GetProcOGL( glUniformMatrix4fv );
        GetProcOGL( glBindAttribLocation );
        GetProcOGL( glEnableVertexAttribArray );
        GetProcOGL( glDisableVertexAttribArray );
        GetProcOGL( glVertexAttribPointer );
        GetProcOGL( glGetProgramiv );
    // Common
        GetProcOGL( glStencilFuncSeparate );
        GetProcOGL( glStencilMaskSeparate );
        GetProcOGL( glStencilOpSeparate );
        GetProcOGL( glBlendFuncSeparate );

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  

        {
        // init default render state
            RenderState::Desc desc;
            desc.colorMask  = COLOR_MASK_ALL;
            desc.depthTest  = true;
            desc.depthWrite = true;
            desc.cullFace   = FACE_BACK;

            defRenderState = createRenderState(desc);
        }
    }

    virtual ~ContextGL() {
        destroyResource(defRenderState);
    }

    virtual void present() override {
        extern HDC osDC;
        SwapBuffers(osDC);
    }

    virtual Buffer* createBuffer(const Buffer::Desc &desc) override {
        return new BufferGL(desc);
    }

    virtual Texture* createTexture(const Texture::Desc &desc) override {
        return new TextureGL(desc);
    }

    virtual Shader* createShader(const Shader::Desc &desc) override {
        return new ShaderGL(desc);
    }

    virtual Mesh* createMesh(const Mesh::Desc &desc) override {
        return new MeshGL(desc);
    }

    virtual RenderState* createRenderState(const RenderState::Desc &desc) override {
        return new RenderStateGL(desc);
    }

    virtual FrameBuffer* createFrameBuffer(const FrameBuffer::Desc &desc) override {
        return new FrameBufferGL(desc);
    }

    virtual void resetRenderState() override {
        setRenderState(defRenderState);
    }

    virtual void clear(int clearMask, const vec4 &color, float depth, int stencil) override {
        GLuint mask = 0;
        if (clearMask & CLEAR_MASK_COLOR) {
            glClearColor(color.x, color.y, color.z, color.w);
            mask |= GL_COLOR_BUFFER_BIT;
        }

        if (clearMask & CLEAR_MASK_DEPTH) {
            glClearDepth(depth);
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        if (clearMask & CLEAR_MASK_STENCIL) {
            glClearStencil(stencil);
            mask |= GL_STENCIL_BUFFER_BIT;
        }

        if (mask) {
            glClear(mask);
        }
    }

    virtual void setViewport(int x, int y, int width, int height) override {
        glViewport(x, y, width, height);
    }

    virtual void setRenderState(const RenderState *state) override {
        if (state == curRenderState) {
            return;
        }

        RenderStateGL *obj = (RenderStateGL*)state;
        obj->bind(curRenderState, stencilRef);

        Context::setRenderState(state);
    }

    virtual void setStencilRef(uint8 ref) override {
        if (stencilRef == ref) {
            return;
        }
        stencilRef = ref;

        setStencilRef(stencilRef);
    }

    virtual void beginPass(const FrameBuffer *fb) override {
        Context::beginPass(fb);
        FrameBufferGL *obj = (FrameBufferGL*)fb;
        obj->bind();
    }

    virtual void setTexture(const Texture *texture, ShaderSampler sampler) override {
        const TextureGL *obj = (TextureGL*)texture;
        obj->bind(sampler);
    }

    virtual void setUniform(ShaderUniform uniform, const vec4 &value, int count = 1) override {
        ASSERT(curRenderState && curRenderState->desc.shader);
        GLint uid = ((ShaderGL*)curRenderState->desc.shader)->uid[uniform];
        if (uid == -1) return;
        glUniform4fv(uid, count, (GLfloat*)&value);
    }

    virtual void setUniform(ShaderUniform uniform, const mat4 &value, int count = 1) override {
        ASSERT(curRenderState && curRenderState->desc.shader);
        GLint uid = ((ShaderGL*)curRenderState->desc.shader)->uid[uniform];
        if (uid == -1) return;
        glUniformMatrix4fv(uid, count, false, (GLfloat*)&value);
    }

    virtual void draw(const Mesh *mesh, int iStart, int iCount) override {
        const MeshGL *obj = (MeshGL*)mesh;

        iStart = iStart >= 0 ? iStart : 0;
        iCount = iCount >= 0 ? iCount : obj->desc.iBuffer->desc.count;

        glBindVertexArray(obj->id);
        glDrawElements(GL_TRIANGLES, iCount, GL_UNSIGNED_SHORT, (Index*)NULL + iStart);
    }
};

#endif
