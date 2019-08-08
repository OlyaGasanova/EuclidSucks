#ifndef H_CONTEXT
#define H_CONTEXT

#include "utils.h"

#define DESC_CTOR\
    Desc() { memset(this, 0, sizeof(*this)); }\
    inline bool operator == (const Desc &desc) { return memcmp(this, &desc, sizeof(Desc)) == 0; }

const vec4 COLOR_BLACK = {0.0f, 0.0f, 0.0f, 0.0f};

struct ContextResource {
    ContextResource() {}
    virtual ~ContextResource() {}
};

typedef unsigned short Index;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec2 texcoord;
    vec4 color;

    Vertex() {}
    Vertex(const vec3 &p, const vec3 &n, const vec2 &t, const vec4 &c) : position(p), normal(n), texcoord(t), color(c) {}
};

enum BufferFlag {
    BUF_INDEX   = (1 << 0),
    BUF_VERTEX  = (1 << 1),
    BUF_DYNAMIC = (1 << 2),
};

struct Buffer : ContextResource {

    struct Desc {
        uint16  flags;
        uint16  stride;
        uint32  count;
        void    *data;

        DESC_CTOR
    } desc;

    Buffer(const Desc &desc) : desc(desc) {}
};

struct Texture : ContextResource {

    enum TextureFlags {
        FLAG_REPEAT   = (1 << 0),
        FLAG_NEAREST  = (1 << 1),
        FLAG_GEN_MIPS = (1 << 2),
        FLAG_CUBEMAP  = (1 << 3),
        FLAG_TARGET   = (1 << 4),
    };

    enum Format {
        FMT_UNKNOWN,
        FMT_R8,
        FMT_RG8,
        FMT_RGBA8,
        FMT_RGBA8_SRGB,
        FMT_R16F,
        FMT_RG16F,
        FMT_RGBA16F,
        FMT_R32F,
        FMT_RG32F,
        FMT_RGB32F,
        FMT_RGBA32F,
        FMT_R11G11B10F,
        FMT_D24S8,
        FMT_BC1,
        FMT_BC1_SRGB,
        FMT_BC2,
        FMT_BC2_SRGB,
        FMT_BC3,
        FMT_BC3_SRGB,
        FMT_BC4,
        FMT_BC5,
        FMT_BC6,
        FMT_BC7,
        FMT_BC7_SRGB,
        FMT_MAX,
    };

    struct Desc {
        uint16  flags;
        uint16  width;
        uint16  height;
        uint16  depth;
        uint16  slices;
        uint16  levels;
        Format  format;
        void    *data;

        Desc(uint16  flags,
             uint16  width,
             uint16  height,
             uint16  depth,
             uint16  slices,
             uint16  levels,
             Format  format,
             void    *data = NULL) : flags(flags), width(width), height(height), depth(depth), slices(slices), levels(levels), format(format), data(data) {}

        DESC_CTOR
    } desc;

    Texture(const Desc &desc) : desc(desc) {}

    virtual void setLabel(const char *text) {}
};


#define DECL_ENUM(v) v,
#define DECL_STR(v)  #v,

#define SHADER_ATTRIBS(E) \
    E( aPosition ) \
    E( aNormal   ) \
    E( aTangent  ) \
    E( aTexCoord ) \
    E( aColor    )

#define SHADER_SAMPLERS(E) \
    E( sDiffuse  ) \
    E( sNormal   ) \
    E( sEnvmap   ) \
    E( sLUT      )

#define SHADER_UNIFORMS(E) \
    E( uViewProj   ) \
    E( uModel      ) \
    E( uViewPos    ) \
    E( uLightPos   ) \
    E( uLightColor ) \
    E( uParams     )

enum ShaderAttrib   { SHADER_ATTRIBS(DECL_ENUM)  aMAX };
enum ShaderSampler  { SHADER_SAMPLERS(DECL_ENUM) sMAX };
enum ShaderUniform  { SHADER_UNIFORMS(DECL_ENUM) uMAX };

struct Shader : ContextResource {

    struct Desc {
        uint32  size;
        void    *data;

        DESC_CTOR
    } desc;

    Shader(const Desc &desc) : desc(desc) {}

    virtual void setLabel(const char *text) {}
};


struct Mesh : ContextResource {

    struct Desc {
        Buffer  *iBuffer;
        Buffer  *vBuffer;
        uint32  vStart;

        DESC_CTOR
    } desc;

    Mesh(const Desc &desc) : desc(desc) {}
};


#define MAX_RENDER_TARGETS 4

struct RenderPass : ContextResource {

    enum Op {
        OP_COLOR_CLEAR    = (1 << 0),
        OP_COLOR_LOAD     = (1 << 1),
        OP_COLOR_STORE    = (1 << 2),

        OP_DEPTH_CLEAR    = (1 << 3),
        OP_DEPTH_LOAD     = (1 << 4),
        OP_DEPTH_STORE    = (1 << 5),

        OP_STENCIL_CLEAR  = (1 << 6),
        OP_STENCIL_LOAD   = (1 << 7),
        OP_STENCIL_STORE  = (1 << 8),
    };
    
    struct Desc {
        Texture::Format rt[MAX_RENDER_TARGETS];
        Texture::Format ds;
        uint16          op;
        vec4            clearColor;
        float           clearDepth;
        uint8           clearStencil;

        Desc(Texture::Format rt0, 
             Texture::Format rt1,
             Texture::Format rt2,
             Texture::Format rt3,
             Texture::Format ds,
             uint16 op, 
             const vec4 &clearColor = COLOR_BLACK,
             float clearDepth = 1.0f,
             uint8 clearStencil = 0x00) : ds(ds), op(op), clearColor(clearColor), clearDepth(clearDepth), clearStencil(clearStencil) {

            rt[0] = rt0;
            rt[1] = rt1;
            rt[2] = rt2;
            rt[3] = rt3;
        }

        DESC_CTOR
    } desc;

    RenderPass(const Desc &desc) : desc(desc) {}
};


struct TargetView {
    Texture* texture;
    uint16   level;
    uint16   face;

    TargetView() {}
    TargetView(Texture* texture, uint16 level = 0, uint16 face = 0) : texture(texture), level(level), face(face) {}

    bool getSize(int &w, int &h) const {
        if (!texture) {
            return false;
        }
        w = texture->desc.width  << level;
        h = texture->desc.height << level;
        return true;
    }
};

struct FrameBuffer : ContextResource {
    int width;
    int height;

    struct Desc {
        RenderPass* pass;
        TargetView  rt[MAX_RENDER_TARGETS];
        TargetView  ds;

        DESC_CTOR
    } desc;

    FrameBuffer(const Desc &desc) : desc(desc) {
        if (desc.ds.getSize(width, height)) {
            return;
        }

        for (int i = 0; i < MAX_RENDER_TARGETS; i++) {
            if (desc.rt[i].getSize(width, height)) {
                return;
            }
        }
    }
};


enum ClearMask {
    CLEAR_MASK_COLOR   = 1,
    CLEAR_MASK_DEPTH   = 2,
    CLEAR_MASK_STENCIL = 4,
    CLEAR_MASK_ALL     = CLEAR_MASK_COLOR | CLEAR_MASK_DEPTH | CLEAR_MASK_STENCIL,
};

enum Face {
    FACE_FRONT,
    FACE_BACK,
    FACE_NONE,
    FACE_MAX = FACE_NONE,
};

enum StencilOp {
    STENCIL_KEEP,
    STENCIL_ZERO,
    STENCIL_REPLACE,
    STENCIL_INC,
    STENCIL_DEC,
};

enum CompareOp {
    COMPARE_ALWAYS,
    COMPARE_NEVER,
    COMPARE_EQUAL,
    COMPARE_NOT_EQUAL,
    COMPARE_LESS,
    COMPARE_LESS_EQUAL,
    COMPARE_GREATER,
    COMPARE_GREATER_EQUAL,
};

enum BlendOp {
    BLEND_ZERO,
    BLEND_ONE,
    BLEND_SRC_COLOR,
    BLEND_ONE_MINUS_SRC_COLOR,
    BLEND_DST_COLOR,
    BLEND_ONE_MINUS_DST_COLOR,
    BLEND_SRC_ALPHA,
    BLEND_ONE_MINUS_SRC_ALPHA,
    BLEND_DST_ALPHA,
    BLEND_ONE_MINUS_DST_ALPHA,
    BLEND_SRC1_COLOR,
    BLEND_ONE_MINUS_SRC1_COLOR,
    BLEND_SRC1_ALPHA,
    BLEND_ONE_MINUS_SRC1_ALPHA,
};

enum ColorMask {
    COLOR_MASK_NONE = 0,
    COLOR_MASK_R    = (1 << 0),
    COLOR_MASK_G    = (1 << 1),
    COLOR_MASK_B    = (1 << 2),
    COLOR_MASK_A    = (1 << 3),
    COLOR_MASK_ALL  = COLOR_MASK_R | COLOR_MASK_G | COLOR_MASK_B | COLOR_MASK_A,
};

struct RenderState : ContextResource {

    struct Desc {

        struct Stencil {
            StencilOp sfail;
            StencilOp zfail;
            StencilOp zpass;
            CompareOp compare;
            uint8     readMask;
            uint8     writeMask;
        };

        struct Blend {
            BlendOp colorOpSrc;
            BlendOp colorOpDst;
            BlendOp alphaOpSrc;
            BlendOp alphaOpDst;
        };

        uint8       colorMask;
        bool        blending;
        bool        depthWrite;
        bool        depthTest;
        bool        stencilTest;
        Stencil     stencil[FACE_MAX];
        Face        cullFace;
        Blend       blend;
        RenderPass  *pass;
        Shader      *shader;

        DESC_CTOR
    } desc;

    RenderState(const Desc &desc) : desc(desc) {}
};


struct Context {
    const RenderState *curRenderState;
    const FrameBuffer *curFrameBuffer;

    int width;
    int height;

    Context() : curRenderState(NULL), curFrameBuffer(NULL) {}
    virtual ~Context() {}

    virtual void present() {};

    virtual Buffer*       createBuffer       (const Buffer::Desc      &desc)  { return NULL; }
    virtual Shader*       createShader       (const Shader::Desc      &desc)  { return NULL; }
    virtual Texture*      createTexture      (const Texture::Desc     &desc)  { return NULL; }
    virtual Mesh*         createMesh         (const Mesh::Desc        &desc)  { return NULL; }
    virtual RenderState*  createRenderState  (const RenderState::Desc &desc)  { return NULL; }
    virtual RenderPass*   createRenderPass   (const RenderPass::Desc  &desc)  { return NULL; }
    virtual FrameBuffer*  createFrameBuffer  (const FrameBuffer::Desc &desc)  { return NULL; }

    virtual void destroyResource(ContextResource *obj) { delete obj; }

    virtual void resetRenderState() {}

    virtual void resize(int width, int height) {
        this->width  = width;
        this->height = height;
    }

    virtual FrameBuffer* getSwapChainFB() {
        return NULL;
    }

    virtual void clear(int clearMask, const vec4 &color = COLOR_BLACK, float depth = 1.0f, int stencil = 0) {}

    virtual void setViewport(int x, int y, int width, int height) {}

    virtual void setRenderState(const RenderState *renderState) {
        curRenderState = renderState;
        //ASSERT(!curFrameBuffer || (renderState && curFrameBuffer && curRenderState->desc.pass == curFrameBuffer->desc.pass));
    }

    virtual void setStencilRef(uint8 ref) {}

    virtual void beginPass(const FrameBuffer *fb) {
        ASSERT(!curFrameBuffer);
        curFrameBuffer = fb;
    }

    virtual void endPass() {
        curFrameBuffer = NULL;
    }

    virtual void setTexture(const Texture *texture, ShaderSampler sampler) {}
    virtual void setUniform(ShaderUniform uniform, const vec4 &value, int count = 1) {}
    virtual void setUniform(ShaderUniform uniform, const mat4 &value, int count = 1) {}

    virtual void draw(const Mesh *mesh, int iStart = -1, int iCount = -1) {}

    virtual void pushMarker(const char *title) {}
    virtual void popMarker() {}
};

#include "context/gl.h"

enum GAPI {
    GAPI_GL,
//    GAPI_D3D11,
//    GAPI_VULKAN,
};

Context *ctx;


struct Marker {

    Marker(const char *title) {
        ctx->pushMarker(title);
    }

    ~Marker() {
        ctx->popMarker();
    }

};

#define GPU_MARKER(title) Marker _marker##__LINE__(title)

#endif
