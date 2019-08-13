#ifndef H_SEGMENT
#define H_SEGMENT

struct Segment {
    vec3 a, b;

    Segment() {}

    Segment(const vec3 &a, const vec3 &b) : a(a), b(b) {}
};

#endif
