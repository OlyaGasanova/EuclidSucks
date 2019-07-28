#ifndef H_MATERIAL
#define H_MATERIAL

#include "utils.h"
#include "context.h"

struct Material {
    Shader  *shader;
    Texture *texture;

    Material(Stream *stream) {
        char name[256];
        char path[256];

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
    }

    ~Material() {
        ctx->destroyTexture(texture);
        ctx->destroyShader(shader);
    }

    void bind() {
        ctx->setShader(shader);
        ctx->setTexture(texture, sDiffuse);
    }
};

#endif
