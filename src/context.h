#ifndef H_CONTEXT
#define H_CONTEXT

#include "utils.h"

typedef unsigned short Index;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    vec4 color;

    Vertex() {}
    Vertex(const vec3 &p, const vec3 &n, const vec2 &t, const vec4 &c) : position(p), normal(n), texcoord(t), color(c) {}
};

enum BufferType {
    BUFFER_TYPE_INDEX,
    BUFFER_TYPE_VERTEX,
};

struct Buffer  {
    BufferType type;
    int        stride;
    int        count;

    Buffer(BufferType type, int stride, int count) : type(type), stride(stride), count(count) {}
    virtual ~Buffer() {}
};


enum TextureFormat {
    TEX_FMT_RGBA8,
};

enum TextureOptions {
    TEX_OPT_REPEAT  = 1,
    TEX_OPT_NEAREST = 2,
    TEX_OPT_MIPMAP  = 4,
};

struct Texture {
    int           width;
    int           height;
    TextureFormat format;
    uint32        opt;

    Texture(int width, int height, TextureFormat format, uint32 opt) : width(width), height(height), format(format), opt(opt) {}
    virtual ~Texture() {}
};


#define DECL_ENUM(v) v,
#define DECL_STR(v)  #v,

#define SHADER_ATTRIBS(E) \
    E( aPosition ) \
    E( aNormal   ) \
    E( aTexCoord ) \
    E( aColor    )

#define SHADER_SAMPLERS(E) \
    E( sDiffuse ) \
    E( sNormal  )

#define SHADER_UNIFORMS(E) \
    E( uViewProj   ) \
    E( uViewPos    ) \
    E( uLightPos   ) \
    E( uLightColor )

enum ShaderAttrib   { SHADER_ATTRIBS(DECL_ENUM)  aMAX };
enum ShaderSampler  { SHADER_SAMPLERS(DECL_ENUM) sMAX };
enum ShaderUniform  { SHADER_UNIFORMS(DECL_ENUM) uMAX };

struct Shader {
    virtual ~Shader() {}
    virtual void setParam(ShaderUniform uniform, const vec4 &value, int count = 1) {}
    virtual void setParam(ShaderUniform uniform, const mat4 &value, int count = 1) {}
};


struct Mesh {
    Buffer *iBuffer;
    Buffer *vBuffer;
    int    iStart;
    int    iCount;
    int    vStart;

    Mesh(Buffer *iBuffer, Buffer *vBuffer, int iStart, int iCount, int vStart) : iBuffer(iBuffer), vBuffer(vBuffer), iStart(iStart), iCount(iCount), vStart(vStart) {}
    virtual ~Mesh() {}
};


enum ClearMask {
    CLEAR_MASK_COLOR   = 1,
    CLEAR_MASK_DEPTH   = 2,
    CLEAR_MASK_STENCIL = 4,
    CLEAR_MASK_ALL     = CLEAR_MASK_COLOR | CLEAR_MASK_DEPTH | CLEAR_MASK_STENCIL,
};

const vec4 COLOR_BLACK = {0.0f, 0.0f, 0.0f, 0.0f};

struct Context {
    int width;
    int height;

    virtual ~Context() {}
    virtual void present() {};

    virtual Buffer*  createBuffer(BufferType type, int stride, int count, const void *data)                                    { return NULL; }
    virtual Shader*  createShader(int size, const void *data) = 0                                                              { return NULL; }
    virtual Texture* createTexture(int width, int height, TextureFormat format, const uint32 opt, int mips, const void **data) { return NULL; }
    virtual Mesh*    createMesh(Buffer *iBuffer, Buffer *vBuffer, int iStart, int iCount, int vStart)                          { return NULL; }

    virtual void destroyBuffer(Buffer *buffer)    { delete buffer;  }
    virtual void destroyShader(Shader *shader)    { delete shader;  }
    virtual void destroyTexture(Texture *texture) { delete texture; }
    virtual void destroyMesh(Mesh *mesh)          { delete mesh;    }

    virtual void resize(int nWidth, int nHeight) { width = nWidth; height = nHeight; }
    virtual void clear(int clearMask, const vec4 &color = COLOR_BLACK, float depth = 1.0f, int stencil = 0) {}
    virtual void setViewport(int x, int y, int width, int height) {}
    virtual void setTexture(const Texture *texture, ShaderSampler sampler) {}
    virtual void setShader(const Shader *shader) {}

    virtual void draw(const Mesh *mesh) {}
};

#include "context/gl.h"

enum GAPI {
    GAPI_GL,
//    GAPI_D3D11,
//    GAPI_VULKAN,
};

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
