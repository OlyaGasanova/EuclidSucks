#ifndef H_PORTAL
#define H_PORTAL

#include "model.h"

struct Portal : Model {
    uint32  warpIndex;
    Portal* warpPortal;
    mat4    warpTransform;
    vec4    clipPlane;

    Portal(Stream *stream) : Model(stream, false), warpPortal(NULL) {
        type = TYPE_PORTAL;
        stream->read(&warpIndex, sizeof(warpIndex));
    }

    void updateTransform() {
        warpTransform = matrix;
        warpTransform.rotateY(PI);
        warpTransform = warpTransform * warpPortal->matrix.inverseOrtho();

        vec3 a = vertices[indices[0]].position;
        vec3 b = vertices[indices[1]].position;
        vec3 c = vertices[indices[2]].position;
        vec3 n = normalize(cross(b - a, c - a));
        clipPlane = matrix.inverseOrtho().transpose() * vec4(n, -dot(a, n));
    }

    bool collide(const vec3 &a, const vec3 &b) {
        mat4 m = matrix.inverseOrtho();
        Segment segment(m * a, m * b); // transform line segment to the portal local space

        for (int i = 0; i < iCount; i += 3) {
            vec3 &v0 = vertices[indices[i + 0]].position;
            vec3 &v1 = vertices[indices[i + 1]].position;
            vec3 &v2 = vertices[indices[i + 2]].position;
            vec3 tuv;

            Triangle tri(v0, v1, v2);
            if (tri.intersect(segment, tuv) && tuv.x < 1.0f) {
                return true;
            }
        }

        return false;
    }
};

#endif
