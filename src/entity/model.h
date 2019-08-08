#ifndef H_MODEL
#define H_MODEL

#include "entity.h"

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

#endif
