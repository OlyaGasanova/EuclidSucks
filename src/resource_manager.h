#ifndef H_RESOURCE_MANAGER
#define H_RESOURCE_MANAGER

#include "context.h"

struct ResourceManager {

    struct Resource {
        char            *name;
        ContextResource *obj;
        int             ref;

        Resource(const char *res_name) : ref(1) {
            ASSERT(res_name && res_name[0]);
            name = new char[strlen(res_name) + 1];
            strcpy(name, res_name);
        }

        ~Resource() {
            delete name;
        }

        void addRef() {
            //
        }

        void release() {
            ASSERT(ref > 0);
            if (--ref == 0) {
                
            }
        }
    };

    Context *ctx;

    ResourceManager(Context *ctx) : ctx(ctx) {}

    ~ResourceManager() {
        //
    }

    Texture* getTexture(const char *name) {
        char path[256];
        strcpy(path, "textures/");
        strcat(path, name);
        strcat(path, ".dds");

        FileStream stream(path, FileStream::MODE_READ);

        Texture::Desc desc;
        loadDDS(&stream, desc);
        if (!(desc.flags & Texture::FLAG_CUBEMAP)) {
            desc.flags |= Texture::FLAG_REPEAT;
        }
        Texture *texture = ctx->createTexture(desc);
        delete[] desc.data;

        return texture;
    }

    void releaseTexture(Texture *obj) {
        ctx->destroyResource(obj);
    }

    Shader* getShader(const char *name) {
        char path[256];
        strcpy(path, "shaders/");
        strcat(path, name);
        strcat(path, ".glsl");

        Shader::Desc desc;
        desc.data = readFile(path, desc.size);
        Shader *shader = ctx->createShader(desc);
        delete[] desc.data;

        return shader;
    }

    void releaseShader(Shader *obj) {
        ctx->destroyResource(obj);
    }

    RenderState* getRenderState(const RenderState::Desc &desc) {
        return ctx->createRenderState(desc);
    }

    void releaseRenderState(RenderState *state) {
        ctx->destroyResource(state);
    }
};

ResourceManager *resourceManager = NULL;

#endif
