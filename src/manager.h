#ifndef H_MANAGER
#define H_MANAGER

#include "utils.h"
#include "context.h"

#define RENDER_PASS(E) \
    E( PASS_OPAQUE      ) \
    E( PASS_TONEMAPPING )

#define RENDER_TECH(E) \
    E( TECH_COMMON      ) \
    E( TECH_SFILL       ) \
    E( TECH_STEST       )

#define RENDER_TARGET(E) \
    E( RT_MAIN_HDR      ) \
    E( RT_MAIN_DEPTH    )

enum Pass {
    RENDER_PASS(DECL_ENUM)
    PASS_MAX,
    PASS_FINAL = PASS_TONEMAPPING
};

enum Tech {
    RENDER_TECH(DECL_ENUM)
    TECH_MAX
};

enum RenderTarget {
    RENDER_TARGET(DECL_ENUM)
    RT_MAX
};

struct Manager {

    struct Resource {
        char            *name;
        ContextResource *obj;
        int             refCount;

        Resource(const char *name, ContextResource *obj) : name(NULL), obj(obj), refCount(1) {
            ASSERT(name && name[0]);
            this->name = new char[strlen(name) + 1];
            strcpy(this->name, name);
        }

        ~Resource() {
            ctx->destroyResource(obj);
            delete name;
        }

        void addRef() {
            refCount++;
        }

        void release() {
            ASSERT(refCount > 0);
            refCount--;
            if (refCount <= 0) {
                delete this;
            }
        }
    };

    Array<Resource*>     textures;
    Array<Resource*>     shaders;

    Texture*             rto[RT_MAX];
    RenderPass*          rpo[PASS_MAX];
    Array<FrameBuffer*>  fbo[PASS_MAX];

    Manager() {
        for (int i = 0; i < RT_MAX; i++) {
            rto[i] = NULL;
        }

        initRenderPasses();
    }

    ~Manager() {
        freeFrameBuffers();
        freeRenderPasses();
        freeRenderTargets();

        ASSERT(textures.length == 0);
        ASSERT(shaders.length == 0);
    }

    void initRenderTargets(int width, int height) {
        const Texture::Desc descs[] = {
        // RT_MAIN_HDR
            Texture::Desc(Texture::FLAG_TARGET | Texture::FLAG_NEAREST, width, height, 0, 0, 1, Texture::FMT_R11G11B10F),
        // RT_MAIN_DEPTH
            Texture::Desc(Texture::FLAG_TARGET | Texture::FLAG_NEAREST, width, height, 0, 0, 1, Texture::FMT_D24S8)
        };

        ASSERT(COUNT(descs) == RT_MAX);

        static const char *RenderTargetName[] = { RENDER_TARGET(DECL_STR) };

        for (int i = 0; i < RT_MAX; i++) {
            rto[i] = ctx->createTexture(descs[i]);
            rto[i]->setLabel(RenderTargetName[i]);
        }
    }

    void freeRenderTargets() {
        for (int i = 0; i < RT_MAX; i++) {
            ctx->destroyResource(rto[i]);
            rto[i] = NULL;
        }
    }

    void initRenderPasses() {
        ASSERT(MAX_RENDER_TARGETS == 4);

        const static RenderPass::Desc descs[] = {
        // PASS_OPAQUE
            RenderPass::Desc(Texture::FMT_R11G11B10F, // rt0
                             Texture::FMT_UNKNOWN,    // rt1
                             Texture::FMT_UNKNOWN,    // rt2
                             Texture::FMT_UNKNOWN,    // rt3
                             Texture::FMT_D24S8,      // ds
                             RenderPass::OP_COLOR_CLEAR | RenderPass::OP_DEPTH_CLEAR | RenderPass::OP_STENCIL_CLEAR | RenderPass::OP_COLOR_STORE | RenderPass::OP_DEPTH_STORE),
        // PASS_TONEMAPPING
            RenderPass::Desc(Texture::FMT_RGBA8,      // rt0
                             Texture::FMT_UNKNOWN,    // rt1
                             Texture::FMT_UNKNOWN,    // rt2
                             Texture::FMT_UNKNOWN,    // rt3
                             Texture::FMT_UNKNOWN,    // ds
                             RenderPass::OP_COLOR_STORE)
        };

        ASSERT(COUNT(descs) == PASS_MAX);

        for (int i = 0; i < PASS_MAX; i++) {
            rpo[i] = ctx->createRenderPass(descs[i]);
        }
    }

    void freeRenderPasses() {
        for (int i = 0; i < PASS_MAX; i++) {
            ctx->destroyResource(rpo[i]);
        }
    }

    void freeFrameBuffers() {
        for (int i = 0; i < PASS_MAX; i++) {
            for (int j = 0; j < fbo[i].length; j++) {
                ctx->destroyResource(fbo[i][j]);
            }
            fbo[i].clear();
        }
    }

    Texture* getRenderTarget(RenderTarget rt) {
        return rt == RT_MAX ? NULL : rto[rt];
    }

    RenderPass* getRenderPass(Pass pass) {
        return pass == PASS_MAX ? NULL : rpo[pass];
    }

    FrameBuffer* getFrameBuffer(Pass pass, const FrameBuffer::Desc &desc) {
        if (pass == PASS_FINAL) {
            return ctx->getSwapChainFB();
        }

        if (desc.pass == NULL) {
            return NULL;
        }

        int index = -1;

        for (int i = 0; i < fbo[pass].length; i++) {
            if (fbo[pass][i]->desc == desc) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            index = fbo[pass].push(ctx->createFrameBuffer(desc));
        }
        
        return fbo[pass][index];
    }

    ContextResource* find(const Array<Resource*> &list, const char *name) {
        for (int i = 0; i < list.length; i++) {
            Resource *res = list[i];
            if (!strcmp(res->name, name)) {
                res->addRef();
                return res->obj;
            }
        }
        return NULL;
    }

    void release(Array<Resource*> &list, ContextResource *obj) {
        if (!obj) {
            return;
        }

        for (int i = 0; i < list.length; i++) {
            Resource *res = list[i];
            if (res->obj == obj) {
                if (res->refCount == 1) {
                    res->release();
                    list.removeFast(i);
                } else {
                    res->release();
                }
                return;
            }
        }
    }

    Texture* getTexture(const char *name, uint32 flags = 0) {
        Texture *texture = (Texture*)find(textures, name);

        if (!texture) {
            char path[256];
            strcpy(path, "textures/");
            strcat(path, name);
            strcat(path, ".dds");

            FileStream stream(path, FileStream::MODE_READ);

            Texture::Desc desc;
            loadDDS(&stream, desc);
            desc.flags |= flags;
            texture = ctx->createTexture(desc);
            delete[] desc.data;

            texture->setLabel(name);

            textures.push(new Resource(name, texture));
        }

        return texture;
    }

    void releaseTexture(ContextResource *obj) {
        release(textures, obj);
    }

    Shader* getShader(const char *name) {
        Shader *shader = (Shader*)find(shaders, name);

        if (!shader) {
            char path[256];
            strcpy(path, "shaders/");
            strcat(path, name);
            strcat(path, ".glsl");

            Shader::Desc desc;
            desc.data = readFile(path, desc.size);
            shader = ctx->createShader(desc);
            delete[] desc.data;

            shader->setLabel(name);

            shaders.push(new Resource(name, shader));
        }

        return shader;
    }

    void releaseShader(ContextResource *obj) {
        release(shaders, obj);
    }
};

Manager *manager = NULL;

#endif
