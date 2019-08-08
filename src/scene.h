#ifndef H_SCENE
#define H_SCENE

#include "utils.h"
#include "context.h"
#include "entity.h"

struct Scene {
    Array<Entity*> entities;

    Camera  *camera;

    Texture *texEnvmap;
    Texture *texLUT;

    float  time;

    Scene(Stream *stream) : time(0.0f) {
        camera = new Camera(vec3(0), vec3(0));

        {
            char buf[256];
            stream->readStr(buf);
            texEnvmap = manager->getTexture(buf);
        }

        texLUT = manager->getTexture("lut");

        int entitiesCount;
        stream->read(&entitiesCount, sizeof(entitiesCount));

        entities.reserve(entitiesCount);

        for (int i = 0; i < entitiesCount; i++) {
            uint32 type;
            stream->read(&type, sizeof(type));

            Entity *entity = NULL;
            switch (type) {
                case Entity::TYPE_MODEL :
                    entity = new Model(stream);
                    break;
                case Entity::TYPE_LIGHT :
                    entity = new Light(stream);
                    break;
                case Entity::TYPE_CAMERA :
                    entity = new Camera(stream);
                    camera->copyFrom((Camera*)entity);
                    break;
                case Entity::TYPE_START  :
                    entity = new Entity(stream, Entity::Type(type));
                    break;
                default : ASSERT(false);
            }

            entities.push(entity);
        }
    }

    ~Scene() {
        manager->releaseTexture(texEnvmap);
        manager->releaseTexture(texLUT);

        for (int i = 0; i < entities.length; i++) {
            delete entities[i];
        }
        
        delete camera;
    }

    void update() {
        camera->control();
        time += osDeltaTime;

        for (int i = 0; i < entities.length; i++) {
            entities[i]->update();
        }
    }

    void render() {
        renderer->setRenderTarget(0, RT_MAIN_HDR);
        renderer->setDepthTarget(RT_MAIN_DEPTH);

        renderer->beginPass(PASS_OPAQUE);

        camera->aspect = (float)renderer->width / (float)renderer->height;
        camera->refresh();

        renderer->mViewProj = camera->mViewProj;

    // test
        renderer->lightColor[0] = vec4(1.0f, 0.2f, 0.2f, 0.0f);
        renderer->lightColor[1] = vec4(1.0f, 1.0f, 1.0f, 0.0f);
        renderer->lightColor[2] = vec4(0.2f, 0.2f, 1.0f, 0.0f);

        renderer->lightPos[0] = vec4(0, 2, -8, 1.0f / 8.0f);
        renderer->lightPos[1] = vec4(cosf(time) * 4.0f, 2, sinf(time) * 4.0f, 1.0f / 16.0f);
        renderer->lightPos[2] = vec4(0, 2, +8, 1.0f / 8.0f);

        renderer->viewPos = vec4(camera->pos, 0.0f);

        ctx->setTexture(texEnvmap, sEnvmap);
        ctx->setTexture(texLUT,    sLUT);
    // ----
        
        for (int i = 0; i < entities.length; i++) {
            Entity *entity = entities[i];

            if (!entity->renderable) {
                continue;
            }

            renderer->render(entity->renderable);
        }

        renderer->endPass();

        // reset targets
        renderer->setRenderTarget(0);
        renderer->setDepthTarget();
    }

    void compose() {
        // ...
        renderer->tonemapping();
    }
};

#endif
