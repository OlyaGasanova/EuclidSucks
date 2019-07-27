#ifndef H_CORE
#define H_CORE


#include "context.h"



extern int osGetTimeMS();

namespace Core {
    bool  isQuit;
    float deltaTime;

    void init() {
        isQuit = false;

    }

    void quit() {
        isQuit = true;
    }
}

#endif
