#ifndef H_MATH_MAT4
#define H_MATH_MAT4

#include "utils.h"
#include "vec3.h"

struct mat4 {
    float e00, e10, e20, e30,
          e01, e11, e21, e31,
          e02, e12, e22, e32,
          e03, e13, e23, e33;

    mat4() {}

    mat4(float s) {
        e10 = e20 = e30 = e01 = e21 = e31 = e02 = e12 = e32 = e03 = e13 = e23 = 0.0f;
        e00 = e11 = e22 = e33 = s;
    }

    vec4& right()  const { return *((vec4*)&e00); }
    vec4& up()     const { return *((vec4*)&e01); }
    vec4& dir()    const { return *((vec4*)&e02); }
    vec4& offset() const { return *((vec4*)&e03); }

    static mat4 frustum(float l, float r, float b, float t, float znear, float zfar) {
        mat4 m;
        m.identity();

        m.e00 = 2.0f * znear / (r - l);
        m.e11 = 2.0f * znear / (t - b);
        m.e02 = (r + l) / (r - l);
        m.e12 = (t + b) / (t - b);
        m.e32 = -1.0f;
        m.e33 = 0.0f;
        m.e22 = (znear + zfar) / (znear - zfar);
        m.e23 = 2.0f * zfar * znear / (znear - zfar);

        return m;
    }

    static mat4 perspective(float fov, float aspect, float znear, float zfar) {
        float y = tanf(fov * 0.5f * DEG2RAD) * znear;
        float x = y;

        if (aspect >= 1.0f) {
            x = y * aspect;
        } else {
            y /= aspect;
        }

        return frustum(-x, x, -y, y, znear, zfar);
    }

    void identity() {
        *this = mat4(1.0f);
    }

    mat4 operator * (const mat4 &m) const {
        mat4 r;
        r.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20 + e03 * m.e30;
        r.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20 + e13 * m.e30;
        r.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20 + e23 * m.e30;
        r.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20 + e33 * m.e30;
        r.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21 + e03 * m.e31;
        r.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21 + e13 * m.e31;
        r.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21 + e23 * m.e31;
        r.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21 + e33 * m.e31;
        r.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22 + e03 * m.e32;
        r.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22 + e13 * m.e32;
        r.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22 + e23 * m.e32;
        r.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22 + e33 * m.e32;
        r.e03 = e00 * m.e03 + e01 * m.e13 + e02 * m.e23 + e03 * m.e33;
        r.e13 = e10 * m.e03 + e11 * m.e13 + e12 * m.e23 + e13 * m.e33;
        r.e23 = e20 * m.e03 + e21 * m.e13 + e22 * m.e23 + e23 * m.e33;
        r.e33 = e30 * m.e03 + e31 * m.e13 + e32 * m.e23 + e33 * m.e33;
        return r;
    }

    void translate(const vec3 &offset) {
        mat4 m;
        m.identity();
        m.setPos(offset);
        *this = *this * m;
    };

    void scale(const vec3 &factor) {
        mat4 m;
        m.identity();
        m.e00 = factor.x;
        m.e11 = factor.y;
        m.e22 = factor.z;
        *this = *this * m;
    }

    void rotateX(float angle) {
        mat4 m;
        m.identity();
        float s, c;
        sincos(angle, &s, &c);
        m.e11 = c;  m.e21 = s;
        m.e12 = -s; m.e22 = c;
        *this = *this * m;
    }

    void rotateY(float angle) {
        mat4 m;
        m.identity();
        float s, c;
        sincos(angle, &s, &c);
        m.e00 = c;  m.e20 = -s;
        m.e02 = s;  m.e22 = c;
        *this = *this * m;
    }

    void rotateZ(float angle) {
        mat4 m;
        m.identity();
        float s, c;
        sincos(angle, &s, &c);
        m.e00 = c;  m.e01 = -s;
        m.e10 = s;  m.e11 = c;
        *this = *this * m;
    }

    vec3 getPos() const {
        return vec3(e03, e13, e23);
    }

    void setPos(const vec3 &pos) {
        e03 = pos.x;
        e13 = pos.y;
        e23 = pos.z;
    }

    vec3 getRot() const {
        vec3 rot;

        // NOT FOR ALL CASES
        rot.x = -atan2f(e12, e02);
        rot.y = -atan2f(e20, e00);
        rot.z =  atan2f(e21, e11);

        return rot;
    }

    void setRot(const vec3 &rot) {
        rotateY(rot.y);
        rotateX(rot.x);
        rotateZ(rot.z);
    }

    mat4 inverseOrtho() const {
        mat4 r;
        r.e00 =  e00; r.e10 = e01; r.e20 = e02; r.e30 = 0;
        r.e01 =  e10; r.e11 = e11; r.e21 = e12; r.e31 = 0;
        r.e02 =  e20; r.e12 = e21; r.e22 = e22; r.e32 = 0;
        r.e03 = -(e03 * e00 + e13 * e10 + e23 * e20);
        r.e13 = -(e03 * e01 + e13 * e11 + e23 * e21);
        r.e23 = -(e03 * e02 + e13 * e12 + e23 * e22);
        r.e33 = 1.0f;
        return r;
    }
};

#endif
