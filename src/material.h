#ifndef H_MATERIAL
#define H_MATERIAL

#include "utils.h"
#include "manager.h"

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
    uint32      stages;
    Texture     *textures[MAX_MATERIAL_TEXTURES];
    RenderState *state;

    Material(Stream *stream) {
        char str[256];
        stream->readStr(str);
        Shader *shader = manager->getShader(str);

        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            stream->readStr(str);
            if (!str[0]) {
                textures[i] = NULL;
                continue;
            }

            textures[i] = manager->getTexture(str, Texture::FLAG_REPEAT);
        }

        stream->read(&params, sizeof(params));

        initState(shader);
    }

    Material(const char *shaderName) {
        Shader *shader = manager->getShader(shaderName);

        memset(textures, 0, sizeof(textures));
        memset(&params, 0, sizeof(params));

        initState(shader);
    }

    ~Material() {
        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            manager->releaseTexture(textures[i]);
        }
 
        manager->releaseShader(state->desc.shader);
        ctx->destroyResource(state);
    }

    void initState(Shader *shader) {
        RenderState::Desc desc;
        desc.colorMask  = COLOR_MASK_ALL;
        desc.depthTest  = true;
        desc.depthWrite = true;
        desc.cullFace   = FACE_BACK;
        desc.pass       = manager->getRenderPass(PASS_OPAQUE);
        desc.shader     = shader;

        state = ctx->createRenderState(desc);
    }

    void setTexture(ShaderSampler sampler, Texture *texture) {
        textures[sampler] = texture;
    }
};

#endif
