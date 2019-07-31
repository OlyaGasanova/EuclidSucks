#ifndef H_MATERIAL
#define H_MATERIAL

#include "utils.h"
#include "context.h"

#define MAX_MATERIAL_TEXTURES 2

struct Material {
    State   *state;
    Texture *textures[MAX_MATERIAL_TEXTURES];

    Material(Stream *stream) {
        char name[256];
        char path[256];

        Shader *shader;

        {
            stream->readStr(name);
            strcpy(path, "shaders/");
            strcat(path, name);

            Shader::Desc desc;
            desc.data = readFile(path, desc.size);

            shader = ctx->createShader(desc);

            delete[] desc.data;
        }

        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            stream->readStr(name);
            if (!name[0]) {
                textures[i] = NULL;
                continue;
            }

            strcpy(path, "textures/");
            strcat(path, name);
            strcat(path, ".dds");
            FileStream stream(path, FileStream::MODE_READ);

            Texture::Desc desc;
            loadDDS(&stream, desc);
            desc.flags |= Texture::FLAG_REPEAT;
            textures[i] = ctx->createTexture(desc);

            delete[] desc.data;
        }

        {
            State::Desc desc;
            desc.colorMask  = COLOR_MASK_ALL;
            desc.depthTest  = true;
            desc.depthWrite = true;
            desc.cullFace   = FACE_BACK;
            desc.shader     = shader;

            state = ctx->createState(desc);
        }
    }

    ~Material() {
        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            ctx->destroyTexture(textures[i]);
        }
        ctx->destroyShader(state->desc.shader);
        ctx->destroyState(state);
    }

    void bind() {
        ctx->setState(state);
        for (int i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
            if (textures[i]) {
                ctx->setTexture(textures[i], ShaderSampler(i));
            }
        }
    }
};

#endif
