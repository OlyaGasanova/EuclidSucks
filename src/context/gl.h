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

    TextureGL(const Texture::Desc &desc) : Texture(desc) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        for (int i = 0; i < desc.levels; i++) {
            glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA8, desc.width >> i, desc.height >> i, 0, GL_RGBA, GL_UNSIGNED_BYTE, desc.data[i]);
        }

        bool genmips = (desc.flags & TEX_GEN_MIPS);
        bool mipmaps = (desc.levels > 1) || genmips;
        bool nearest = (desc.flags & TEX_NEAREST);
        bool repeat  = (desc.flags & TEX_REPEAT);

        GLint wrap      = repeat  ? GL_REPEAT : GL_CLAMP_TO_EDGE;
        GLint filter    = nearest ? GL_NEAREST : GL_LINEAR;
        GLint filterMip = mipmaps ? (nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR) : filter;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMip);

        if (genmips) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    virtual ~TextureGL() {
        glDeleteTextures(1, &id);
    }
};

struct ShaderGL : Shader {
    GLuint id;
    GLint  uid[uMAX];

    ShaderGL(const Shader::Desc &desc) : Shader(desc) {
        static const char *ShaderAttribName[aMAX]  = { SHADER_ATTRIBS(DECL_STR)  };
        static const char *ShaderSamplerName[sMAX] = { SHADER_SAMPLERS(DECL_STR) };
        static const char *ShaderUniformName[uMAX] = { SHADER_UNIFORMS(DECL_STR) };

        const char *VS_DEFINES = "#version 110\n" "#define VS\n";
        const char *FS_DEFINES = "#version 110\n" "#define FS\n" "#define fragColor gl_FragColor\n";

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

    virtual void setParam(ShaderUniform uniform, const vec4 &value, int count) override {
        if (uid[uniform] == -1) return;
        glUniform4fv(uid[uniform], count, (GLfloat*)&value);
    }

    virtual void setParam(ShaderUniform uniform, const mat4 &value, int count) override {
        if (uid[uniform] == -1) return;
        glUniformMatrix4fv(uid[uniform], count, false, (GLfloat*)&value);
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
        glEnableVertexAttribArray(aTexCoord);
        glEnableVertexAttribArray(aColor);

        glVertexAttribPointer(aPosition, 3, GL_FLOAT, false, vbuf->desc.stride, &v->position);
        glVertexAttribPointer(aNormal,   3, GL_FLOAT, false, vbuf->desc.stride, &v->normal);
        glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, false, vbuf->desc.stride, &v->texcoord);
        glVertexAttribPointer(aColor,    4, GL_FLOAT, false, vbuf->desc.stride, &v->color);

        glBindVertexArray(0);
    }

    virtual ~MeshGL() {
        glDeleteVertexArrays(1, &id);
    }
};

struct ContextGL : Context {
    ContextGL() {
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
    }

    virtual ~ContextGL() {
        //
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

    virtual void setTexture(const Texture *texture, ShaderSampler sampler) override {
        const TextureGL *tex = (TextureGL*)texture;
        glActiveTexture(GL_TEXTURE0 + sampler);
        glBindTexture(GL_TEXTURE_2D, tex->id);
    }

    virtual void setShader(const Shader *shader) override {
        const ShaderGL *shd = (ShaderGL*)shader;
        glUseProgram(shd->id);
    }

    virtual void draw(const Mesh *mesh, int iStart, int iCount) override {
        const MeshGL *msh = (MeshGL*)mesh;

        iStart = iStart >= 0 ? iStart : 0;
        iCount = iCount >= 0 ? iCount : msh->desc.iBuffer->desc.count;

        glBindVertexArray(msh->id);
        glDrawElements(GL_TRIANGLES, iCount, GL_UNSIGNED_SHORT, (Index*)NULL + iStart);
    }
};

#endif
