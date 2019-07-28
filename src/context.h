#ifndef H_CONTEXT
#define H_CONTEXT

#include "utils.h"

#define DESC_CTOR Desc() { memset(this, 0, sizeof(*this)); }

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

struct Buffer  {

    struct Desc {
        uint16  flags;
        uint16  stride;
        uint32  count;
        void    *data;

        DESC_CTOR
    } desc;

    Buffer(const Desc &desc) : desc(desc) {}
    virtual ~Buffer() {}
};


enum TextureFlags {
    TEX_REPEAT   = (1 << 0),
    TEX_NEAREST  = (1 << 1),
    TEX_GEN_MIPS = (1 << 2),
    TEX_CUBEMAP  = (1 << 3),
};

struct Texture {

    enum Format {
        R8,
        RG8,
        RGBA8,
        R16F,
        RG16F,
        RGBA16F,
        D24S8,
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

        DESC_CTOR
    } desc;

    Texture(const Desc &desc) : desc(desc) {}
    virtual ~Texture() {}
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
    E( sDiffuse ) \
    E( sNormal  )

#define SHADER_UNIFORMS(E) \
    E( uViewProj   ) \
    E( uModel      ) \
    E( uLightDir   ) \
    E( uLightColor )

enum ShaderAttrib   { SHADER_ATTRIBS(DECL_ENUM)  aMAX };
enum ShaderSampler  { SHADER_SAMPLERS(DECL_ENUM) sMAX };
enum ShaderUniform  { SHADER_UNIFORMS(DECL_ENUM) uMAX };

struct Shader {

    struct Desc {
        uint32  size;
        void    *data;

        DESC_CTOR
    } desc;

    Shader(const Desc &desc) : desc(desc) {}
    virtual ~Shader() {}
};


struct Mesh {

    struct Desc {
        Buffer  *iBuffer;
        Buffer  *vBuffer;
        uint32  vStart;

        DESC_CTOR
    } desc;

    Mesh(const Desc &desc) : desc(desc) {}
    virtual ~Mesh() {}
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

const vec4 COLOR_BLACK = {0.0f, 0.0f, 0.0f, 0.0f};

struct State {

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

        uint8     colorMask;
        bool      blending;
        bool      depthWrite;
        bool      depthTest;
        bool      stencilTest;
        Stencil   stencil[FACE_MAX];
        Face      cullFace;
        Blend     blend;
        Shader    *shader;

        DESC_CTOR
    } desc;

    State(const Desc &desc) : desc(desc) {}
    virtual ~State() {}
};

struct Context {
    int width;
    int height;

    virtual ~Context() {}
    virtual void present() {};

    virtual Buffer*  createBuffer   (const Buffer::Desc  &desc)  { return NULL; }
    virtual Shader*  createShader   (const Shader::Desc  &desc)  { return NULL; }
    virtual Texture* createTexture  (const Texture::Desc &desc)  { return NULL; }
    virtual Mesh*    createMesh     (const Mesh::Desc    &desc)  { return NULL; }
    virtual State*   createState    (const State::Desc   &desc)  { return NULL; }

    virtual void destroyBuffer   (Buffer  *buffer)   { delete buffer;  }
    virtual void destroyShader   (Shader  *shader)   { delete shader;  }
    virtual void destroyTexture  (Texture *texture)  { delete texture; }
    virtual void destroyMesh     (Mesh    *mesh)     { delete mesh;    }
    virtual void destroyState    (State   *state)    { delete state;   }

    virtual void resetState() {}

    virtual void resize(int nWidth, int nHeight) { width = nWidth; height = nHeight; }
    virtual void clear(int clearMask, const vec4 &color = COLOR_BLACK, float depth = 1.0f, int stencil = 0) {}

    virtual void setViewport(int x, int y, int width, int height) {}
    virtual void setTexture(const Texture *texture, ShaderSampler sampler) {}
    virtual void setState(const State *state) {}
    virtual void setStencilRef(uint8 ref) {}

    virtual void setUniform(ShaderUniform uniform, const vec4 &value, int count = 1) {}
    virtual void setUniform(ShaderUniform uniform, const mat4 &value, int count = 1) {}

    virtual void draw(const Mesh *mesh, int iStart = -1, int iCount = -1) {}
};

#include "context/gl.h"

enum GAPI {
    GAPI_GL,
//    GAPI_D3D11,
//    GAPI_VULKAN,
};

Context *ctx = NULL;

Context* createContext(GAPI gapi) {
    switch (gapi) {
        case GAPI_GL : return new ContextGL();
        default      : ASSERT(false);
    }
    return NULL;
}

void destroyContext(Context *context) {
    delete context;
}

#endif
