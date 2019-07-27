#ifndef H_MATH_VEC3
#define H_MATH_VEC3

struct vec3 {
    float x, y, z;

    vec3() {}
    vec3(float s) : x(s), y(s), z(s) {};
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

vec3 operator + (const vec3 &a, const vec3 &b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3 operator - (const vec3 &a, const vec3 &b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec3 operator * (const vec3 &a, const vec3 &b) {
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

vec3 cross(const vec3 &a, const vec3 &b) {
    return vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

float dot(const vec3 &a, const vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float length2(const vec3 &v) {
    return dot(v, v);
}

inline float length(const vec3 &v) {
    return sqrtf(length2(v));
}

inline vec3 normalize(const vec3 &v) {
    return v * (1.0f / length(v));
}

vec3 lerp(const vec3 &a, const vec3 &b, const float t) {
    if (t <= 0.0f) return a;
    if (t >= 1.0f) return b;
    return a + (b - a) * t; 
}

#endif
