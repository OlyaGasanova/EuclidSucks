#ifndef H_PORTAL
#define H_PORTAL

#include "model.h"

struct Portal : Model {
    uint32  warpIndex;
    Portal* warpEntity;
    mat4    warpTransform;

    Portal(Stream *stream) : Model(stream, false), warpEntity(NULL) {
        type = TYPE_PORTAL;
        stream->read(&warpIndex, sizeof(warpIndex));
    }

    virtual void update() override {
        Model::update();

        warpTransform = matrix;
        warpTransform.rotateY(PI);
        warpTransform = warpTransform * warpEntity->matrix.inverseOrtho();
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

    // http://www.terathon.com/code/oblique.html
    mat4 getObliqueViewProj(const mat4 &mProj, const mat4 &mView) {
        return mProj * mView;
        // TODO
        vec3 n = vec3(1, 0, 0);
        vec3 v = vertices[0].position;
        vec4 cp = mProj * (mView * vec4(n, -dot(v, n)));

        if (cp.w > 0.0f) {
            return mProj * mView;
        }

        cp = cp * (1.0f / cp.w);

        vec4 q;
        q.x = (sign(cp.x) + mProj.e02) / mProj.e00;
        q.x = (sign(cp.y) + mProj.e12) / mProj.e11;
        q.z = -1.0f;
        q.w = (1.0f + mProj.e22) / mProj.e23;

        vec4 c = cp * (2.0f / dot(cp, q));

        mat4 m = mProj;
        m.e20 = c.x;
        m.e21 = c.y;
        m.e22 = c.z + 1.0f;
        m.e23 = c.w;

        return m * mView;
    }
};

#endif
