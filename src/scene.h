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
    Texture *texLUT;

    float  time;

    Scene(Stream *stream) : time(0.0f) {
        camera = new Camera(vec3(0), vec3(0));

        {
            char buf[256];
            stream->readStr(buf);
            texEnvmap = resourceManager->getTexture(buf);
        }

        texLUT = resourceManager->getTexture("lut");

        stream->read(&entitiesCount, sizeof(entitiesCount));
        entities = new Entity*[entitiesCount];

        for (int i = 0; i < entitiesCount; i++) {
            uint32 type;
            stream->read(&type, sizeof(type));

            switch (type) {
                case Entity::TYPE_MODEL :
                    entities[i] = new Model(stream);
                    break;
                case Entity::TYPE_LIGHT :
                    entities[i] = new Light(stream);
                    break;
                case Entity::TYPE_CAMERA :
                    entities[i] = new Camera(stream);
                    camera->copyFrom((Camera*)entities[i]);
                    break;
                case Entity::TYPE_START  :
                    entities[i] = new Entity(stream, Entity::Type(type));
                    break;
                default : ASSERT(false);
            }
        }
    }

    ~Scene() {
        resourceManager->releaseTexture(texEnvmap);
        resourceManager->releaseTexture(texLUT);

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

        renderer->viewPos = vec4(camera->pos, 0.0f);

        ctx->setTexture(texEnvmap, sEnvmap);
        ctx->setTexture(texLUT,    sLUT);
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
