#ifndef H_ENTITY
#define H_ENTITY

#include "utils.h"
#include "material.h"
#include "renderable.h"

struct Entity {

    enum Type {
        TYPE_MODEL,
        TYPE_LIGHT,
        TYPE_CAMERA,
        TYPE_START,
    };

    Entity     *next;

    Type       type;
    mat4       matrix;
    Renderable *renderable;

    Entity(Stream *stream, Type type) : type(type), renderable(NULL) {
        if (stream) {
            stream->read(&matrix, sizeof(matrix));
        }
    }

    virtual ~Entity() {
        delete renderable;
    }

    virtual void update() {};
};

#include "entity/camera.h"
#include "entity/model.h"
#include "entity/light.h"

#endif
