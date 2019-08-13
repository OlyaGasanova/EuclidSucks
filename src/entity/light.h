#ifndef H_LIGHT
#define H_LIGHT

#include "entity.h"

struct Light : Entity {

    Light(Stream *stream) : Entity(stream, TYPE_LIGHT) {

    }

    virtual void update() override {
        // TODO rotate sun
    }
};


#endif
