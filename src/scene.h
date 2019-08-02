#ifndef H_SCENE
#define H_SCENE

#include "utils.h"
#include "context.h"
#include "entity.h"

struct Scene {
    Camera *camera;

    int32  entitiesCount;
    Entity **entities;

    Texture *texEnvmap;

    float  time;

    Scene(Stream *stream) : time(0.0f) {
        camera = new Camera(vec3(-11, 9, 0), vec3(0, -PI * 0.5f, 0));

        {
            char buf[256];
            stream->readStr(buf);
            texEnvmap = resourceManager->getTexture(buf);
        }

        stream->read(&entitiesCount, sizeof(entitiesCount));
        entities = new Entity*[entitiesCount];

        for (int i = 0; i < entitiesCount; i++) {
            uint32 type;
            stream->read(&type, sizeof(type));

            switch (type) {
                case Entity::TYPE_MODEL :
                    entities[i] = new Model(stream);
                    break;
                case Entity::TYPE_SUN :
                    entities[i] = new Sun(stream);
                    break;
                case Entity::TYPE_START :
                    entities[i] = new Entity(stream, Entity::Type(type));
                    break;
                default : ASSERT(false);
            }
        }

        //if (start >= 0) {
        //    camera->pos = entities[start]->matrix.getPos();
        //}

        camera->pos = camera->pos + vec3(0.0f, 1.0f, 0.0f);
    }

    ~Scene() {
        resourceManager->releaseTexture(texEnvmap);

        for (int i = 0; i < entitiesCount; i++) {
            delete entities[i];
        }
        delete[] entities;

        delete camera;
    }

    void update() {
        camera->control();
        time += osDeltaTime;

        for (int i = 0; i < entitiesCount; i++) {
            entities[i]->update();
        }
    }

    void render() {
        camera->aspect = (float)renderer->width / (float)renderer->height;
        camera->refresh();

        renderer->mViewProj = camera->mViewProj;

    // test
        renderer->lightColor[0] = vec4(1.0f, 0.2f, 0.2f, 0.0f);
        renderer->lightColor[1] = vec4(1.0f, 1.0f, 1.0f, 0.0f);
        renderer->lightColor[2] = vec4(0.2f, 0.2f, 1.0f, 0.0f);

        renderer->lightPos[0] = vec4(-8, 2, 0, 1.0f / 8.0f);
        renderer->lightPos[1] = vec4(cosf(time) * 4.0f, 2, sinf(time) * 4.0f, 1.0f / 16.0f);
        renderer->lightPos[2] = vec4(+8, 2, 0, 1.0f / 8.0f);

        renderer->viewPos = vec4(camera->pos.x, camera->pos.y, camera->pos.z, 0);

        ctx->setTexture(texEnvmap, sEnvmap);
    // ----
        
        for (int i = 0; i < entitiesCount; i++) {
            Entity *entity = entities[i];

            if (!entity->renderable) {
                continue;
            }

            renderer->render(entity->renderable);
        }
    }
};

#endif
