#ifndef H_TRIANGLE
#define H_TRIANGLE

#include "vec3.h"
#include "segment.h"

struct Triangle {
    vec3 a, b, c;

    Triangle() {}

    Triangle(const vec3 &a, const vec3 &b, const vec3 &c) : a(a), b(b), c(c) {}

    bool intersect(const Segment &segment, vec3 &tuv) const {
        vec3 ab = b - a;
        vec3 ac = c - a;
        vec3 ss = segment.b - segment.a;
        vec3 nn = cross(ss, ac);
        float d = dot(nn, ab);

        if (d <= 0.0f) {
            return false;
        }

        vec3 r = segment.a - a;
        tuv.y = dot(nn, r);
        if (tuv.y < 0.0f || tuv.y > d) {
            return false;
        }

        vec3 q = cross(r, ab);
        tuv.z = dot(ss, q);
        if (tuv.z < 0.0f || tuv.y + tuv.z > d) {
            return false;
        }

        d = 1.0f / d;
        tuv.x = dot(q, ac);
        if (tuv.x <= 0.0f) {
            return false;
        }

        tuv = tuv * d;
        return true;
    }
};

#endif
