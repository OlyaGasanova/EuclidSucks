#ifndef H_RENDERABLE
#define H_RENDERABLE

#include "utils.h"
#include "context.h"

struct Renderable {
    Mesh     *mesh;
    Material *material;

    // instance specific data
    mat4     matrix;

    Renderable() : mesh(NULL), material(NULL) {}

    virtual ~Renderable() {
        if (mesh) {
            ctx->destroyResource(mesh->desc.iBuffer);
            ctx->destroyResource(mesh->desc.vBuffer);
            ctx->destroyResource(mesh);
        }
        delete material;
    }
};

#endif