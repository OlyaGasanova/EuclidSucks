#ifndef H_RENDERER
#define H_RENDERER

#include "utils.h"
#include "context.h"
#include "material.h"
#include "entity.h"
#include "resource_manager.h"

#define MAX_LIGHTS  3
#define MAX_BATCHES 4096

struct Renderer {

    enum Pass {
        PASS_DEFAULT,
        PASS_MAX,
    };

    int     width;
    int     height;

    Pass    pass;

    vec4 lightColor[MAX_LIGHTS];
    vec4 lightPos[MAX_LIGHTS];
    vec4 viewPos;
    mat4 mViewProj;

    Renderer(GAPI gapi) : pass(PASS_DEFAULT) {
        switch (gapi) {
            case GAPI_GL : ctx = new ContextGL(); break;
            default      : ASSERT(false);
        }

        resourceManager = new ResourceManager(ctx);

        ASSERT(MAX_RENDER_TARGETS == 4);

        RenderPass::Desc descs[] = {
        // PASS_DEFAULT
            RenderPass::Desc(Texture::FMT_RGBA8, Texture::FMT_UNKNOWN, Texture::FMT_UNKNOWN, Texture::FMT_UNKNOWN, Texture::FMT_UNKNOWN,
                             RenderPass::OP_COLOR_CLEAR | RenderPass::OP_DEPTH_CLEAR | RenderPass::OP_STENCIL_CLEAR | RenderPass::OP_COLOR_STORE)
        };

        ASSERT(COUNT(descs) == PASS_MAX);

        for (int i = 0; i < PASS_MAX; i++) {
            renderPass[i] = ctx->createRenderPass(descs[i]);
        }
    }

    ~Renderer() {
        for (int i = 0; i < PASS_MAX; i++) {
            ctx->destroyResource(renderPass[i]);
        }
        delete resourceManager;
        delete ctx;
    }

    void resize(int width, int height) {
        this->width  = width;
        this->height = height;
        ctx->resize(width, height);
    }

    RenderPass* getRenderPass(Pass pass) {
        return renderPass[pass];
    }

    void begin() {
        ctx->resetRenderState();
        ctx->setViewport(0, 0, width, height);
        ctx->clear(CLEAR_MASK_ALL, vec4(0.4f, 0.7f, 1.0f, 1.0));
    }

    void end() {
        ctx->present();
    }

    void setMaterial(Material *material) {
        ctx->setRenderState(material->state);
        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            if (material->textures[i]) {
                ctx->setTexture(material->textures[i], ShaderSampler(i));
            }
        }

        ctx->setUniform(uParams, *((vec4*)&material->params), sizeof(Material::Params) / sizeof(vec4));

    // set material common uniforms
        ctx->setUniform(uViewProj,   mViewProj);
        ctx->setUniform(uViewPos,    viewPos);
        ctx->setUniform(uLightPos,   lightPos[0],   MAX_LIGHTS);
        ctx->setUniform(uLightColor, lightColor[0], MAX_LIGHTS);
    }

    void render(Renderable *renderable) {
        setMaterial(renderable->material);
        
    // set model params
        ctx->setUniform(uModel, renderable->matrix);

        ctx->draw(renderable->mesh);
    }

private:
    RenderPass *renderPass[PASS_MAX];
};

Renderer *renderer = NULL;

#endif
