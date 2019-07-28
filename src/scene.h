#ifndef H_SCENE
#define H_SCENE

#include "utils.h"
#include "context.h"
#include "material.h"

struct Scene {

    struct Object {

        enum {
            TYPE_MESH,
            TYPE_SUN,
            TYPE_START,
        };

        int32    type;
        mat4     matrix;
        Mesh     *mesh;
        Material *material;

        Object(Stream *stream) : mesh(NULL), material(NULL) {
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

        ~Object() {
            if (mesh) {
                ctx->destroyBuffer(mesh->desc.iBuffer);
                ctx->destroyBuffer(mesh->desc.vBuffer);
                ctx->destroyMesh(mesh);
            }
            delete material;
        }
    };

    Camera *camera;

    int32  objectsCount;
    Object **objects;

    int    start;
    int    sun;

    Scene(Stream *stream) : start(-1), sun(-1) {
        camera = new Camera(vec3(0.0f), vec3(0.0f));

        stream->read(&objectsCount, sizeof(objectsCount));
        objects = new Object*[objectsCount];

        for (int i = 0; i < objectsCount; i++) {
            objects[i] = new Object(stream);
            
            switch (objects[i]->type) {
                case Object::TYPE_MESH :
                    break;
                case Object::TYPE_SUN :
                    sun = i;
                    break;
                case Object::TYPE_START :
                    start = i;
                    break;
                default : ASSERT(false);
            }
        }

        if (start >= 0) {
            camera->pos = objects[start]->matrix.getPos();
        }

        camera->pos = camera->pos + vec3(0.0f, 1.0f, 0.0f);
    }

    ~Scene() {
        for (int i = 0; i < objectsCount; i++) {
            delete objects[i];
        }
        delete[] objects;

        delete camera;
    }

    void update() {
        camera->control();
    }

    void render() {
        camera->aspect = (float)ctx->width / (float)ctx->height;
        camera->update();

        vec4 lightColor = vec4(1.0f, 0.9f, 0.8f, 0.0f);
        vec4 lightDir   = vec4(0.3333f);

        if (sun >= 0) {
            lightDir = objects[sun]->matrix.inverseOrtho().dir();
        }

        for (int i = 0; i < objectsCount; i++) {
            Object *obj = objects[i];

            if (obj->type != Object::TYPE_MESH) {
                continue;
            }

            obj->material->bind();

            obj->material->shader->setParam(uViewProj,   camera->mViewProj);
            obj->material->shader->setParam(uModel,      obj->matrix);
            obj->material->shader->setParam(uLightDir,   lightDir);
            obj->material->shader->setParam(uLightColor, lightColor);


            ctx->draw(objects[i]->mesh);
        }
    }
};

#endif
