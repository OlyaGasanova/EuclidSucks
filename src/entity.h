#ifndef H_ENTITY
#define H_ENTITY

#include "utils.h"
#include "context.h"
#include "material.h"

struct Entity {

    enum {
        TYPE_MESH,
        TYPE_SUN,
        TYPE_START,
    };

    int32    type;
    mat4     matrix;
    Mesh     *mesh;
    Material *material;

    Entity(Stream *stream) : mesh(NULL), material(NULL) {
        stream->read(&type, sizeof(type));
        stream->read(&matrix, sizeof(matrix));

        if (type == TYPE_MESH) {
            // read material
            material = new Material(stream);

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

                mesh = ctx->createMesh(desc);
            }
        }
    }

    ~Entity() {
        if (mesh) {
            ctx->destroyBuffer(mesh->desc.iBuffer);
            ctx->destroyBuffer(mesh->desc.vBuffer);
            ctx->destroyMesh(mesh);
        }
        delete material;
    }
};

#endif
