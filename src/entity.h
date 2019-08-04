#ifndef H_ENTITY
#define H_ENTITY

#include "utils.h"
#include "material.h"
#include "renderable.h"

struct Entity {

    enum Type {
        TYPE_MODEL,
        TYPE_LIGHT,
        TYPE_CAMERA,
        TYPE_START,
    };

    Type       type;
    mat4       matrix;
    Renderable *renderable;

    Entity(Stream *stream, Type type) : type(type), renderable(NULL) {
        if (stream) {
            stream->read(&matrix, sizeof(matrix));
        }
    }

    virtual ~Entity() {
        delete renderable;
    }

    virtual void update() {};
};


struct Model : Entity {
    
    Model(Stream *stream) : Entity(stream, TYPE_MODEL) {
        renderable = new Renderable();

        // read material
        renderable->material = new Material(stream);

        Buffer *iBuffer;
        Buffer *vBuffer;

        { // read indices
            int32 count;
            stream->read(&count, sizeof(count));
            Index *indices = new Index[count];
            stream->read(indices, count * sizeof(Index));

            Buffer::Desc desc;
            desc.flags  = BUF_INDEX;
            desc.count  = count;
            desc.stride = sizeof(Index);
            desc.data   = indices;

            iBuffer = ctx->createBuffer(desc);

            delete[] indices;
        }

        { // read vertices
            int32 count;
            stream->read(&count, sizeof(count));
            Vertex *vertices = new Vertex[count];
            stream->read(vertices, count * sizeof(Vertex));

            Buffer::Desc desc;
            desc.flags  = BUF_VERTEX;
            desc.count  = count;
            desc.stride = sizeof(Vertex);
            desc.data   = vertices;

            vBuffer = ctx->createBuffer(desc);

            delete[] vertices;
        }

        { // create mesh
            Mesh::Desc desc;
            desc.iBuffer = iBuffer;
            desc.vBuffer = vBuffer;
            desc.vStart  = 0;

            renderable->mesh = ctx->createMesh(desc);
        }
    }

    virtual void update() override {
        renderable->matrix = matrix;
    }
};


struct Light : Entity {

    Light(Stream *stream) : Entity(stream, TYPE_LIGHT) {

    }

    virtual void update() override {
        // TODO rotate sun
    }
};

#endif
