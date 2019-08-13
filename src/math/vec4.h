#ifndef H_MATH_VEC4
#define H_MATH_VEC4

#include "vec3.h"

struct vec4 {
    float x, y, z, w;

    vec4() {}
    vec4(float s) : x(s), y(s), z(s), w(s) {};
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    vec4(const vec3 &v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

    operator vec3() const { return vec3(x, y, z); };
};

vec4 operator * (const vec4 &a, float s) {
    return vec4(a.x * s, a.y * s, a.z * s, a.w * s);
}

float dot(const vec4 &a, const vec4 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

#endif
