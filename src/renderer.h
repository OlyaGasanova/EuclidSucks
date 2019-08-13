#ifndef H_RENDERER
#define H_RENDERER

#include "utils.h"
#include "context.h"
#include "material.h"
#include "entity.h"
#include "manager.h"

#define MAX_LIGHTS  3
#define MAX_BATCHES 4096

struct Renderer {
    FrameBuffer::Desc  fboDesc;

    int  width;
    int  height;

    Pass pass;
    Tech tech;

    vec4 lightColor[MAX_LIGHTS];
    vec4 lightPos[MAX_LIGHTS];
    vec4 viewPos;
    mat4 mViewProj;

    Renderable *rndTonemapping;

    Renderer(GAPI gapi) : pass(PASS_MAX), tech(TECH_MAX) {
        switch (gapi) {
            case GAPI_GL : ctx = new ContextGL(); break;
            default      : ASSERT(false);
        }

        manager = new Manager();

        { // Tonemapping
            rndTonemapping = new Renderable();
            rndTonemapping->material = new Material("tonemap");

            Buffer *iBuffer;
            Buffer *vBuffer;

            { // read indices
                Index indices[] = { 0, 1, 2, 0, 2, 3 };

                Buffer::Desc desc;
                desc.flags  = BUF_INDEX;
                desc.count  = COUNT(indices);
                desc.stride = sizeof(Index);
                desc.data   = indices;

                iBuffer = ctx->createBuffer(desc);
            }

            { // read vertices
                Vertex vertices[] = {
                    Vertex(vec3(-1, -1, 0), vec3(0), vec2(0), vec4(0)),
                    Vertex(vec3(+1, -1, 0), vec3(0), vec2(0), vec4(0)),
                    Vertex(vec3(+1, +1, 0), vec3(0), vec2(0), vec4(0)),
                    Vertex(vec3(-1, +1, 0), vec3(0), vec2(0), vec4(0))
                };

                Buffer::Desc desc;
                desc.flags  = BUF_VERTEX;
                desc.count  = COUNT(vertices);
                desc.stride = sizeof(Vertex);
                desc.data   = vertices;

                vBuffer = ctx->createBuffer(desc);
            }

            { // create mesh
                Mesh::Desc desc;
                desc.iBuffer = iBuffer;
                desc.vBuffer = vBuffer;
                desc.vStart  = 0;

                rndTonemapping->mesh = ctx->createMesh(desc);
            }
        }
    }

    ~Renderer() {
        delete rndTonemapping;

        delete manager;
        delete ctx;
    }

    void resize(int width, int height) {
        if (this->width == width && this->height == height) {
            return;
        }

        this->width  = width;
        this->height = height;
        ctx->resize(width, height);

        manager->freeFrameBuffers();
        manager->freeRenderTargets();
        manager->initRenderTargets(width, height);
    }

    void begin() {
        ctx->resetRenderState();
    }

    void end() {
        ctx->present();
    }

    void setRenderTarget(int output, RenderTarget rt = RT_MAX, uint16 level = 0, uint16 face = 0) {
        fboDesc.rt[output] = TargetView(manager->getRenderTarget(rt), level, face);
    }

    void setDepthTarget(RenderTarget rt = RT_MAX, uint16 level = 0, uint16 face = 0) {
        fboDesc.ds = TargetView(manager->getRenderTarget(rt), level, face);
    }

    void beginPass(Pass pass) {
        ASSERT(this->pass == PASS_MAX);
        this->pass = pass;

        static const char *RenderPassName[] = { RENDER_PASS(DECL_STR) };
        ctx->pushMarker(RenderPassName[pass]);

        fboDesc.pass = manager->getRenderPass(pass);

        FrameBuffer *fb = manager->getFrameBuffer(pass, fboDesc);
        ctx->beginPass(fb);
    }

    void endPass() {
        ASSERT(this->pass != PASS_MAX);
        this->pass = PASS_MAX;

        ctx->endPass();
        ctx->popMarker();
    }

    void setMaterial(Material *material) {
        RenderState *state = material->getRenderState(pass, tech);

        if (!state) {
            ASSERT(false);
            return;
        }

        ctx->setRenderState(state);
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

    void tonemapping() {
        beginPass(PASS_TONEMAPPING);
        rndTonemapping->material->setTexture(sDiffuse, manager->getRenderTarget(RT_MAIN_HDR));
        render(rndTonemapping);
        rndTonemapping->material->setTexture(sDiffuse, NULL);
        endPass();
    }
};

Renderer *renderer = NULL;

#endif
