#ifndef H_SCENE
#define H_SCENE

#include "utils.h"
#include "context.h"
#include "entity.h"

#define MAX_LIGHTS 3

struct Scene {
    Camera *camera;

    int32  objectsCount;
    Entity **entities;

    Texture *texEnvmap;

    int    start;
    int    sun;

    float  time;

    Scene(Stream *stream) : start(-1), sun(-1), time(0.0f) {
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

    // test
        {
            FileStream stream("textures/envmap.dds", FileStream::MODE_READ);

            Texture::Desc desc;
            loadDDS(&stream, desc);
            texEnvmap = ctx->createTexture(desc);

            delete[] desc.data;
        }
    // ----
    }

    ~Scene() {
        ctx->destroyTexture(texEnvmap);

        for (int i = 0; i < objectsCount; i++) {
            delete entities[i];
        }
        delete[] entities;

        delete camera;
    }

    void update() {
        camera->control();
        time += osDeltaTime;
    }

    void render() {
        camera->aspect = (float)ctx->width / (float)ctx->height;
        camera->update();

    // test
        vec4 lightColor[MAX_LIGHTS] = {
             vec4(1.0f, 0.2f, 0.2f, 0.0f),
             vec4(1.0f, 1.0f, 1.0f, 0.0f),
             vec4(0.2f, 0.2f, 1.0f, 0.0f)
        };

        vec4 lightPos[MAX_LIGHTS] = {
            vec4(-8, 2, 0, 1.0f / 8.0f),
            vec4( 0, 2, 0, 1.0f / 16.0f),
            vec4(+8, 2, 0, 1.0f / 8.0f)
        };

        lightPos[1].x += cosf(time) * 4.0f;
        lightPos[1].z += sinf(time) * 4.0f;
    // ----

        vec4 viewPos(camera->pos.x, camera->pos.y, camera->pos.z, 0);
        
        for (int i = 0; i < objectsCount; i++) {
            Entity *entity = entities[i];

            if (entity->type != Entity::TYPE_MESH) {
                continue;
            }

            entity->material->bind();

            ctx->setTexture(texEnvmap, sEnvmap);

            ctx->setUniform(uViewProj,   camera->mViewProj);
            ctx->setUniform(uModel,      entity->matrix);
            ctx->setUniform(uViewPos,    viewPos);
            ctx->setUniform(uLightPos,   lightPos[0],   MAX_LIGHTS);
            ctx->setUniform(uLightColor, lightColor[0], MAX_LIGHTS);

            ctx->draw(entities[i]->mesh);
        }
    }
};

#endif
