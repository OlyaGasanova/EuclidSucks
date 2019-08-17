#ifndef H_CAMERA
#define H_CAMERA

#include "utils.h"
#include "entity.h"

struct Camera : Entity {
    vec3  pos;
    vec3  rot;
    float fov;
    float aspect;
    float znear;
    float zfar;

    mat4  mProj;
    mat4  mView;

    Camera(const vec3 &pos, const vec3 &rot) : Entity(NULL, Entity::TYPE_CAMERA), pos(pos), rot(rot), fov(90.0f), znear(0.1f), zfar(1024.0f) {
        matrix.identity();
        mView.identity();
    }

    Camera(Stream *stream) : Entity(stream, Entity::TYPE_CAMERA) {
        stream->read(&fov,   sizeof(fov));
        stream->read(&znear, sizeof(znear));
        stream->read(&zfar,  sizeof(zfar));

        fov *= RAD2DEG;
    }

    void copyFrom(const Camera *camera) {
        matrix = camera->matrix;
        fov    = camera->fov;
        znear  = camera->znear;
        zfar   = camera->zfar;

    // TODO use quaternions + pos + uniform scale for all transforms
        matrix.rotateX(-PI * 0.5f);
        matrix.rotateZ(PI);

        pos    = matrix.getPos();
        rot    = matrix.getRot();
    }

    void refresh() {
        mProj = mat4::perspective(fov, aspect, znear, zfar);
        mView = matrix.inverseOrtho();
    }

    void setOblique(const vec4 &clipPlane) { // http://www.terathon.com/code/oblique.html
        vec4 p = mView.inverseOrtho().transpose() * clipPlane;

        vec4 q;
        q.x = (sign(p.x) + mProj.e02) / mProj.e00;
        q.y = (sign(p.y) + mProj.e12) / mProj.e11;
        q.z = -1.0f;
        q.w = (1.0f + mProj.e22) / mProj.e23;

        vec4 c = p * (2.0f / dot(p, q));

        mProj.e20 = c.x;
        mProj.e21 = c.y;
        mProj.e22 = c.z + 1.0f;
        mProj.e23 = c.w;
    }

    mat4 getViewProj() {
        return mProj * mView;
    }

    void control() {
        if (Input::down[MOUSE_L]) {
            vec3 d = Input::mouseStartL - Input::mousePos;
            Input::mouseStartL = Input::mousePos;
            rot.x += d.y * 0.01f;
            rot.y += d.x * 0.01f;

            rot.x = clamp(rot.x, -PI * 0.5f, PI * 0.5f);
        }

        vec3 D = vec3(matrix.dir());
        vec3 R = vec3(matrix.right());

        float speed = 2.0f * osDeltaTime;

        if (Input::down[KEY_SHIFT]) speed *= 4.0f;
        if (Input::down[KEY_CTRL])  speed /= 4.0f;
        if (Input::down[KEY_W]) pos = pos - D * speed;
        if (Input::down[KEY_S]) pos = pos + D * speed;
        if (Input::down[KEY_A]) pos = pos - R * speed;
        if (Input::down[KEY_D]) pos = pos + R * speed;

        matrix.identity();
        matrix.setPos(pos);
        matrix.setRot(rot);
    }
};

#endif
