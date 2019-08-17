#ifndef H_UTILS
#define H_UTILS

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

typedef signed char    int8;
typedef signed short   int16;
typedef signed int     int32;

#include <windows.h>
#include <stdio.h>

#define LOG(...) printf(__VA_ARGS__)
#define ASSERT(expr) if (expr) {} else { LOG("ASSERT:\n  %s:%d\n  %s => %s\n", __FILE__, __LINE__, __FUNCTION__, #expr); __debugbreak(); }


// system
extern int    osGetTimeMS();
extern float  osDeltaTime;
extern bool   osClose;


// math
#include <math.h>

#define PI      3.14159265358979323846f
#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

#define COUNT(arr) (int(sizeof(arr) / sizeof(arr[0])))

template <class T>
inline void swap(T &a, T &b) {
    T tmp = a;
    a = b;
    b = tmp;
}

template <typename T>
inline const T& min(const T &a, const T &b) {
    return a < b ? a : b;
}

template <class T>
inline const T& max(const T &a, const T &b) {
    return a > b ? a : b;
}

template <class T>
inline const T sign(const T &x) {
    return (T)(x > 0 ? 1 : (x < 0 ? -1 : 0));
}

void sincos(float r, float *s, float *c) {
    *s = sinf(r);
    *c = cosf(r);
}

float clamp(float x, float minVal, float maxVal) {
    if (x < minVal) return minVal;
    if (x > maxVal) return maxVal;
    return x;
}

struct Color32 {
    uint8 r, g, b, a;
};

template <class T>
void qsort(T* v, int L, int R) {
    int i = L;
    int j = R;
    const T m = v[(L + R) / 2];

    while (i <= j) {
        while (T::cmp(v[i], m) < 0) i++;
        while (T::cmp(m, v[j]) < 0) j--;

        if (i <= j)
            swap(v[i++], v[j--]);
    }

    if (L < j) qsort(v, L, j);
    if (i < R) qsort(v, i, R);
}

template <class T>
void sort(T *items, int count) {
    if (count > 1) {
        qsort(items, 0, count - 1);
    }
}


#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"
#include "math/segment.h"
#include "math/triangle.h"

#include "common/stream.h"
#include "common/array.h"
#include "common/dds.h"

// file system
char* readFile(const char *name, uint32 &size) {
    FileStream stream(name, FileStream::MODE_READ);
    size = stream.size;

    char *data = new char[size];
    stream.read(data, size);

    return data;
}

#endif
