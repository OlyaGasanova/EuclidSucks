#ifndef H_CAMERA
#define H_CAMERA

#include "utils.h"

struct Camera {
    vec3  pos;
    vec3  rot;
    float fov;
    float aspect;
    float znear;
    float zfar;

    mat4  mProj;
    mat4  mView;
    mat4  mViewProj;

    Camera(const vec3 &pos, const vec3 &rot) : pos(pos), rot(rot), fov(90.0f), znear(0.1f), zfar(1024.0f) {}

    void update() {
        mProj = mat4::perspective(fov, aspect, znear, zfar);
        
        mView.identity();
        mView.rotateZ(-rot.z);
        mView.rotateX(-rot.x);
        mView.rotateY(-rot.y);
        mView.translate(vec3(-pos.x, -pos.y, -pos.z));

        mViewProj = mProj * mView;
    }

    void control() {
        if (Input::down[MOUSE_L]) {
            vec3 d = Input::mouseStartL - Input::mousePos;
            Input::mouseStartL = Input::mousePos;
            rot.x += d.y * 0.01f;
            rot.y += d.x * 0.01f;

            rot.x = clamp(rot.x, -PI * 0.5f, PI * 0.5f);
        }

        mat4 mViewInv = mView.inverseOrtho();
        vec3 D = vec3(mViewInv.dir());
        vec3 R = vec3(mViewInv.right());
        float speed = 2.0f * osDeltaTime;

        if (Input::down[KEY_SHIFT]) speed *= 4.0f;
        if (Input::down[KEY_CTRL])  speed /= 4.0f;
        if (Input::down[KEY_W]) pos = pos - D * speed;
        if (Input::down[KEY_S]) pos = pos + D * speed;
        if (Input::down[KEY_A]) pos = pos - R * speed;
        if (Input::down[KEY_D]) pos = pos + R * speed;
    }
};

#endif
