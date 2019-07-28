#ifndef H_GAME
#define H_GAME

#include "input.h"
#include "context.h"
#include "utils.h"
#include "camera.h"
#include "scene.h"

#define MAX_TIME_STEP_MS 20

namespace Game {
    int     lastTime;
    int     fpsTime;
    int     frame;

    Scene   *scene;

    void init() {
        ctx = createContext(GAPI_GL);

        lastTime = osGetTimeMS();
        fpsTime = lastTime;
        frame = 0;

        FileStream stream("scenes/scene1.scn", FileStream::MODE_READ);
        scene = new Scene(&stream);
    }

    void deinit() {
        delete scene;

        destroyContext(ctx);
    }

    void updateStep() {
        scene->update();
    }

    bool update() {
        int time  = osGetTimeMS();
        int delta = time - lastTime;
        lastTime = time;

        if (delta <= 0) {
            return false;
        }

        while (delta > 0) {
            int t = min(MAX_TIME_STEP_MS, delta);
            osDeltaTime = t * 0.001f;
            updateStep();
            delta -= t;
        }

        return true;
    }

    void render() {
        ctx->resetState();
        ctx->setViewport(0, 0, ctx->width, ctx->height);
        ctx->clear(CLEAR_MASK_ALL, vec4(0.4f, 0.7f, 1.0f, 1.0));

        scene->render();

        ctx->present();

    // fps counter
        frame++;
        if (lastTime - fpsTime > 1000) {
            fpsTime = lastTime;
            LOG("FPS: %d\n", frame);
            frame = 0;
        }
    }
}

#endif
