#include "vector.h"
#include <math.h>

Vec3 add(Vec3 v1, Vec3 v2) {
    Vec3 r = {v1.x+v2.x, v1.y+v2.y, v1.z+v2.z};
    return r;
}

Vec3 subtract(Vec3 v1, Vec3 v2) {
    Vec3 r = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    return r;
}

Vec3 scalarMultiply(float s, Vec3 v) {
    Vec3 r = {s*v.x, s*v.y, s*v.z};
    return r;
}

Vec3 scalarDivide(Vec3 v, float d) {
    Vec3 r = {v.x/d, v.y/d, v.z/d};
    return r;
}

Vec3 normalize(Vec3 v) {
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len == 0.0f) {
        Vec3 zero = {0,0,0};
        return zero;
    }
    return scalarDivide(v, len);
}

float dot(Vec3 v1, Vec3 v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vec3 cross(Vec3 v1, Vec3 v2) {
    Vec3 r;
    r.x = v1.y*v2.z - v1.z*v2.y;
    r.y = v1.z*v2.x - v1.x*v2.z;
    r.z = v1.x*v2.y - v1.y*v2.x;
    return r;
}

float length2(Vec3 v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

float length(Vec3 v) {
    return sqrtf(length2(v));
}

float distance2(Vec3 v1, Vec3 v2) {
    Vec3 d = subtract(v1,v2);
    return length2(d);
}

float distance(Vec3 v1, Vec3 v2) {
    return sqrtf(distance2(v1,v2));
}
