#ifndef H_GAME
#define H_GAME

#include "input.h"
#include "renderer.h"
#include "utils.h"
#include "scene.h"

#define MAX_TIME_STEP_MS 20

namespace Game {
    int     lastTime;
    int     fpsTime;
    int     frame;

    Scene   *scene;

    void init() {
        renderer = new Renderer(GAPI_GL);

        lastTime = osGetTimeMS();
        fpsTime = lastTime;
        frame = 0;

        FileStream stream("scenes/test_pbr.scn", FileStream::MODE_READ);
        scene = new Scene(&stream);
    }

    void deinit() {
        delete scene;
        delete renderer;
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
        GPU_MARKER("Frame");

        renderer->begin();

        scene->render();
        scene->compose();

        renderer->end();

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
