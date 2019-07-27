#ifndef H_MATH_VEC4
#define H_MATH_VEC4

struct vec4 {
    float x, y, z, w;

    vec4() {}
    vec4(float s) : x(s), y(s), z(s), w(s) {};
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    operator vec3() const { return vec3(x, y, z); };
};

#endif
