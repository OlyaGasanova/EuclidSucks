#ifndef H_MATERIAL
#define H_MATERIAL

#include "utils.h"
#include "context.h"

struct Material {
    State   *state;
    Texture *texture;

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

        {
            stream->readStr(name);
            strcpy(path, "textures/");
            strcat(path, name);
            FileStream stream(path, FileStream::MODE_READ);

            Texture::Desc desc;
            loadTGA(&stream, desc);
            desc.flags |= TEX_REPEAT | TEX_GEN_MIPS;
            texture = ctx->createTexture(desc);

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
        ctx->destroyTexture(texture);
        ctx->destroyShader(state->desc.shader);
        ctx->destroyState(state);
    }

    void bind() {
        ctx->setState(state);
        ctx->setTexture(texture, sDiffuse);
    }
};

#endif
