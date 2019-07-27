#ifndef H_GAME
#define H_GAME

#include "input.h"
#include "context.h"
#include "utils.h"
#include "camera.h"

Context *ctx = NULL;

#define MAX_TIME_STEP_MS 20

namespace Game {
    int      lastTime;
    Camera  *camera;

// test
    Buffer  *iBuffer;
    Buffer  *vBuffer;
    Mesh    *mesh;
    Texture *texture;
    Shader  *shader;
// ----

    void init() {
        ctx = createContext(GAPI_GL);

        lastTime = osGetTimeMS();

        camera = new Camera(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f));

    // test
        { // test geometry
            {
                const Index indices[] = { 0, 1, 2, 0, 2, 3 };

                Buffer::Desc desc;
                desc.flags  = BUF_INDEX;
                desc.count  = 6;
                desc.stride = sizeof(Index);
                desc.data   = indices;

                iBuffer = ctx->createBuffer(desc);
            }

            {
                const Vertex vertices[] = {
                    Vertex( vec3(-32, 0, -32), vec3(0, 1, 0), vec2( 0,  0), vec4(1, 1, 1, 1) ), 
                    Vertex( vec3( 32, 0, -32), vec3(0, 1, 0), vec2(32,  0), vec4(1, 1, 1, 1) ),
                    Vertex( vec3( 32, 0,  32), vec3(0, 1, 0), vec2(32, 32), vec4(1, 1, 1, 1) ),
                    Vertex( vec3(-32, 0,  32), vec3(0, 1, 0), vec2( 0, 32), vec4(1, 1, 1, 1) ),
                };

                Buffer::Desc desc;
                desc.flags  = BUF_VERTEX;
                desc.count  = 4;
                desc.stride = sizeof(Vertex);
                desc.data   = vertices;

                vBuffer = ctx->createBuffer(desc);
            }

            {
                Mesh::Desc desc;
                desc.iBuffer = iBuffer;
                desc.vBuffer = vBuffer;
                desc.vStart  = 0;

                mesh = ctx->createMesh(desc);
            }
        }

        { // test texture
            uint32 data[64 * 64];
            for (int y = 0; y < 64; y++) {
                for (int x = 0; x < 64; x++) {
                    data[y * 64 + x] = (((x / 32) ^ (y / 32)) & 1) ? 0xFF808080 : 0xFFC0C0C0;
                }
            }
            const void *mips[] = { data };

            Texture::Desc desc;
            desc.flags   = TEX_REPEAT | TEX_GEN_MIPS;
            desc.width   = 64;
            desc.height  = 64;
            desc.levels  = 1;
            desc.format  = Texture::RGBA8;
            desc.data    = mips;

            texture = ctx->createTexture(desc);
        }

        { // test shader
            Shader::Desc desc;
            desc.data = readFile("shaders/base.glsl", desc.size);

            shader = ctx->createShader(desc);

            delete[] desc.data;
        }
    // ----
    }

    void deinit() {
        delete camera;
        ctx->destroyMesh(mesh);
        ctx->destroyShader(shader);
        ctx->destroyBuffer(iBuffer);
        ctx->destroyBuffer(vBuffer);
        ctx->destroyTexture(texture);

        destroyContext(ctx);
    }

    void updateStep() {
        // TODO update player collision, camera position and orientation
        camera->aspect = (float)ctx->width / (float)ctx->height;
        camera->control();
        camera->update();
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
        ctx->setViewport(0, 0, ctx->width, ctx->height);
        ctx->clear(CLEAR_MASK_ALL, vec4(0.4f, 0.7f, 1.0f, 1.0));

    // test
        ctx->setShader(shader);
        ctx->setTexture(texture, sDiffuse);
        shader->setParam(uViewProj, camera->mViewProj);
        ctx->draw(mesh);
    // ----

        ctx->present();
    }
}

#endif
