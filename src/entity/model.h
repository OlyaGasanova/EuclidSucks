#ifndef H_MODEL
#define H_MODEL

#include "entity.h"

struct Model : Entity {
    int32  iCount;
    int32  vCount;
    Index  *indices;
    Vertex *vertices;

    Model(Stream *stream, bool resident = true) : Entity(stream, TYPE_MODEL) {
        renderable = new Renderable();

        // read material
        renderable->material = new Material(stream);

        Buffer *iBuffer;
        Buffer *vBuffer;

        { // read indices
            stream->read(&iCount, sizeof(iCount));
            indices = new Index[iCount];
            stream->read(indices, iCount * sizeof(Index));

            Buffer::Desc desc;
            desc.flags  = BUF_INDEX;
            desc.count  = iCount;
            desc.stride = sizeof(Index);
            desc.data   = indices;

            iBuffer = ctx->createBuffer(desc);

            if (resident) {
                delete[] indices;
                indices = NULL;
            }
        }

        { // read vertices
            stream->read(&vCount, sizeof(vCount));
            vertices = new Vertex[vCount];
            stream->read(vertices, vCount * sizeof(Vertex));

            Buffer::Desc desc;
            desc.flags  = BUF_VERTEX;
            desc.count  = vCount;
            desc.stride = sizeof(Vertex);
            desc.data   = vertices;

            vBuffer = ctx->createBuffer(desc);

            if (resident) {
                delete[] vertices;
                vertices = NULL;
            }
        }

        { // create mesh
            Mesh::Desc desc;
            desc.iBuffer = iBuffer;
            desc.vBuffer = vBuffer;
            desc.vStart  = 0;

            renderable->mesh = ctx->createMesh(desc);
        }
    }

    virtual ~Model() {
        delete[] indices;
        delete[] vertices;
    }

    virtual void update() override {
        renderable->matrix = matrix;
    }
};

#endif
