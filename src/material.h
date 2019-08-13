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
    Shader      *shader;
    Texture     *textures[MAX_MATERIAL_TEXTURES];
    RenderState *states[PASS_MAX][TECH_MAX];

    Material(Stream *stream) {
        char str[256];
        stream->readStr(str);

        shader = manager->getShader(str);

        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            stream->readStr(str);
            if (!str[0]) {
                textures[i] = NULL;
                continue;
            }

            textures[i] = manager->getTexture(str, Texture::FLAG_REPEAT);
        }

        stream->read(&params, sizeof(params));
        memset(states, 0, sizeof(states));
    }

    Material(const char *shaderName) {
        shader = manager->getShader(shaderName);

        memset(textures, 0, sizeof(textures));
        memset(&params, 0, sizeof(params));
        memset(states, 0, sizeof(states));
    }

    ~Material() {
        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            manager->releaseTexture(textures[i]);
        }
 
        for (int pass = 0; pass < PASS_MAX; pass++) {
            for (int tech = 0; tech < TECH_MAX; tech++) {
                RenderState *state = states[pass][tech];
                if (state) {
                    ctx->destroyResource(state);
                }
            }
        }

        manager->releaseShader(shader);
    }

    RenderState* getRenderState(Pass pass, Tech tech) {
        if (states[pass][tech]) {
            return states[pass][tech];
        }

        RenderState::Desc desc;
        desc.colorMask  = COLOR_MASK_ALL;
        desc.depthTest  = true;
        desc.depthWrite = true;
        desc.cullFace   = FACE_BACK;
        desc.pass       = manager->getRenderPass(pass);
        desc.shader     = shader;

        switch (tech) {
            case TECH_COMMON :
                break;
            case TECH_SFILL  :
                desc.colorMask   = COLOR_MASK_NONE;
                desc.stencilTest = true;
                desc.stencil[FACE_FRONT].sfail     = STENCIL_KEEP;
                desc.stencil[FACE_FRONT].zfail     = STENCIL_KEEP;
                desc.stencil[FACE_FRONT].zpass     = STENCIL_REPLACE;
                desc.stencil[FACE_FRONT].compare   = COMPARE_ALWAYS;
                desc.stencil[FACE_FRONT].readMask  = 0xFF;
                desc.stencil[FACE_FRONT].writeMask = 0xFF;
                break;
            case TECH_STEST  :
                desc.stencilTest = true;
                desc.stencil[FACE_FRONT].sfail     = STENCIL_KEEP;
                desc.stencil[FACE_FRONT].zfail     = STENCIL_KEEP;
                desc.stencil[FACE_FRONT].zpass     = STENCIL_KEEP;
                desc.stencil[FACE_FRONT].compare   = COMPARE_EQUAL;
                desc.stencil[FACE_FRONT].readMask  = 0xFF;
                desc.stencil[FACE_FRONT].writeMask = 0xFF;
                break;
            default : ASSERT(false);
        }

        return states[pass][tech] = ctx->createRenderState(desc);
    }

    void setTexture(ShaderSampler sampler, Texture *texture) {
        textures[sampler] = texture;
    }
};

#endif
