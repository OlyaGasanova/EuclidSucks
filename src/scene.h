#ifndef H_SCENE
#define H_SCENE

#include "utils.h"
#include "context.h"
#include "entity.h"

struct Scene {
    Camera *camera;

    int32  objectsCount;
    Entity **entities;

    int    start;
    int    sun;

    Scene(Stream *stream) : start(-1), sun(-1) {
        camera = new Camera(vec3(0.0f), vec3(0.0f));

        stream->read(&objectsCount, sizeof(objectsCount));
        entities = new Entity*[objectsCount];

        for (int i = 0; i < objectsCount; i++) {
            entities[i] = new Entity(stream);
            
            switch (entities[i]->type) {
                case Entity::TYPE_MESH :
                    break;
                case Entity::TYPE_SUN :
                    sun = i;
                    break;
                case Entity::TYPE_START :
                    start = i;
                    break;
                default : ASSERT(false);
            }
        }

        if (start >= 0) {
            camera->pos = entities[start]->matrix.getPos();
        }

        camera->pos = camera->pos + vec3(0.0f, 1.0f, 0.0f);
    }

    ~Scene() {
        for (int i = 0; i < objectsCount; i++) {
            delete entities[i];
        }
        delete[] entities;

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
            lightDir = entities[sun]->matrix.inverseOrtho().dir();
        }

        for (int i = 0; i < objectsCount; i++) {
            Entity *entity = entities[i];

            if (entity->type != Entity::TYPE_MESH) {
                continue;
            }

            entity->material->bind();

            ctx->setUniform(uViewProj,   camera->mViewProj);
            ctx->setUniform(uModel,      entity->matrix);
            ctx->setUniform(uLightDir,   lightDir);
            ctx->setUniform(uLightColor, lightColor);

            ctx->draw(entities[i]->mesh);
        }
    }
};

#endif
