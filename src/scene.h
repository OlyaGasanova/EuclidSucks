#ifndef H_SCENE
#define H_SCENE

#include "utils.h"
#include "context.h"
#include "entity.h"

#define MAX_RECURSION_DEPTH 1

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
                case Entity::TYPE_PORTAL :
                    entity = new Portal(stream);
                    break;
                default : ASSERT(false);
            }

            entities.push(entity);
        }

        // linking portals
        for (int i = 0; i < entitiesCount; i++) {
            Entity *entity = entities[i];
            if (entity->type == Entity::TYPE_PORTAL) {
                Portal* portal = (Portal*)entity;
                portal->warpPortal = (Portal*)entities[portal->warpIndex];
                portal->updateTransform();
            }
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
        time += osDeltaTime;

        vec3 a = camera->matrix.getPos();
        camera->control();
        vec3 b = camera->matrix.getPos();

        bool moved = a.x != b.x || a.y != b.y || a.z != b.z;

        for (int i = 0; i < entities.length; i++) {
            entities[i]->update();

            // check camera vs portal collision
            if (moved && entities[i]->type == Entity::TYPE_PORTAL) {
                Portal *portal = (Portal*)entities[i];
                if (portal->collide(a, b)) {
                    camera->matrix = (camera->matrix.inverseOrtho() * portal->warpTransform).inverseOrtho();
                    camera->pos   = camera->matrix.getPos();
                    camera->rot.y = camera->matrix.getRot().y;
                }
            }
        }

        vec3 r = camera->rot;
        vec3 p = camera->matrix.getRot();
    }

    void renderEntities(int recursionDepth, uint32 portalIndex) {
        GPU_MARKER("Entities");
    // render meshes
        for (int i = 0; i < entities.length; i++) {
            Entity *entity = entities[i];

            entity->flags.visible = false;

            if (!entity->renderable || entity->flags.hidden) {
                continue;
            }

            // TODO frustum culling
            entity->flags.visible = true;

            if (entity->type == Entity::TYPE_PORTAL) {
                continue;
            }

            renderer->render(entity->renderable);
        }

        if (recursionDepth == 0) {
            return;
        }

    // render portals
        uint32 index = portalIndex;

        {
            GPU_MARKER("Mask Portals");
            renderer->setTech(TECH_PORTAL_FILL);

            for (int i = 0; i < entities.length; i++) {
                Entity *entity = entities[i];

                if (!entity->flags.visible || entity->type != Entity::TYPE_PORTAL) {
                    continue;
                }

                ctx->setStencilRef(index++); // set portal index to fill
                renderer->render(entity->renderable);
            }
        }

        uint32 nextPortalIndex = index;
        index = portalIndex;

        {
            GPU_MARKER("Portals");
            renderer->setTech(TECH_PORTAL_TEST);
            
            ctx->clear(CLEAR_MASK_DEPTH);

            for (int i = 0; i < entities.length; i++) {
                Entity *entity = entities[i];

                if (!entity->flags.visible || entity->type != Entity::TYPE_PORTAL) {
                    continue;
                }

                Portal *portal = (Portal*)entity;

                mat4 mProj = camera->mProj;
                mat4 mView = camera->mView;

                camera->mView = camera->mView * portal->warpTransform;

                camera->setOblique(portal->warpPortal->clipPlane);
                renderer->mViewProj = camera->getViewProj();
                renderer->viewPos = vec4(camera->mView.inverseOrtho().getPos(), 1.0);

                ctx->setStencilRef(index++); // set portal index for stencil test

                entity->flags.hidden = true;
                renderEntities(recursionDepth - 1, nextPortalIndex);
                entity->flags.hidden = false;

                camera->mProj = mProj;
                camera->mView = mView;
            }
        }
    }

    void render() {
        GPU_MARKER("Scene");

        renderer->setRenderTarget(0, RT_MAIN_HDR);
        renderer->setDepthTarget(RT_MAIN_DEPTH);

        renderer->beginPass(PASS_OPAQUE);

        camera->aspect = (float)renderer->width / (float)renderer->height;
        camera->refresh();

        renderer->mViewProj = camera->getViewProj();

    // test
        renderer->lightColor[0] = vec4(1.0f, 0.2f, 0.2f, 0.0f);
        renderer->lightColor[1] = vec4(1.0f, 1.0f, 1.0f, 0.0f);
        renderer->lightColor[2] = vec4(0.2f, 0.2f, 1.0f, 0.0f);

        renderer->lightPos[0] = vec4(0, 2, -8, 1.0f / 8.0f);
        renderer->lightPos[1] = vec4(cosf(time) * 4.0f, 2, sinf(time) * 4.0f, 1.0f / 16.0f);
        renderer->lightPos[2] = vec4(0, 2, +8, 1.0f / 8.0f);

        renderer->viewPos = vec4(camera->matrix.getPos(), 0.0f);

        ctx->setTexture(texEnvmap, sEnvmap);
        ctx->setTexture(texLUT,    sLUT);
    // ----

        renderer->setTech(TECH_COMMON);
        renderEntities(MAX_RECURSION_DEPTH, 1);

        renderer->endPass();

        // reset targets
        renderer->setRenderTarget(0);
        renderer->setDepthTarget();
    }

    void compose() {
        GPU_MARKER("Compose");

        renderer->tonemapping();
    }
};

#endif
