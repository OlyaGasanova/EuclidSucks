#ifndef H_MATERIAL
#define H_MATERIAL

#include "utils.h"
#include "resource_manager.h"

#define MAX_MATERIAL_TEXTURES 3

struct Material {

    struct Params {
        vec4  color;
        float metallic;
        float roughness;
        float reserved0;
        float reserved1;
    };

    Params      params;
    RenderState *state;
    Texture     *textures[MAX_MATERIAL_TEXTURES];

    Material(Stream *stream) {
        char name[256];

        Shader *shader;

        stream->readStr(name);
        shader = resourceManager->getShader(name);

        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            stream->readStr(name);
            if (!name[0]) {
                textures[i] = NULL;
                continue;
            }

            textures[i] = resourceManager->getTexture(name);
        }

        stream->read(&params, sizeof(params));

        {
            RenderState::Desc desc;
            desc.colorMask  = COLOR_MASK_ALL;
            desc.depthTest  = true;
            desc.depthWrite = true;
            desc.cullFace   = FACE_BACK;
            //desc.pass       = renderer->getRenderPass(Renderer::PASS_DEFAULT);
            desc.shader     = shader;

            state = resourceManager->getRenderState(desc);
        }
    }

    ~Material() {
        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            resourceManager->releaseTexture(textures[i]);
        }
        resourceManager->releaseShader(state->desc.shader);
        resourceManager->releaseRenderState(state);
    }
};

#endif
