#ifndef H_MATH_VEC2
#define H_MATH_VEC2

struct vec2 {
    float x, y;

    vec2() {}
    vec2(float s) : x(s), y(s) {};
    vec2(float x, float y) : x(x), y(y) {}
};

#endif
