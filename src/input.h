#ifndef H_INPUT
#define H_INPUT

#include "utils.h"

enum InputKey {
// keyboard
    KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_CTRL, KEY_ALT, KEY_SHIFT, KEY_SPACE, KEY_RETURN, KEY_TAB, KEY_ESCAPE,
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
    KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
// mouse
    MOUSE_L, MOUSE_R, MOUSE_M,
    
    KEY_MAX,
};

namespace Input {
    bool down[KEY_MAX];
    vec3 mousePos;
    vec3 mouseStartL;
    vec3 mouseStartR;
    vec3 mouseStartM;

    void setDown(InputKey key, bool state) {
        if (key >= KEY_MAX) return;
        down[key] = state;

        if (state) {
            switch (key) {
                case MOUSE_L : mouseStartL = mousePos;
                case MOUSE_R : mouseStartR = mousePos;
                case MOUSE_M : mouseStartM = mousePos;
            }
        }
    }

    void setPos(InputKey key, const vec3 &pos) {
        switch (key) {
            case MOUSE_L :
            case MOUSE_R :
            case MOUSE_M : mousePos = pos;
        }
    }

    void reset() {
        memset(down, 0, sizeof(down));
        mousePos = 0.0f;
    }
}

#endif
