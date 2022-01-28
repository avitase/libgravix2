#include "linalg.h"
#include <math.h>

double dot(struct Vec3D a, struct Vec3D b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double mag(struct Vec3D v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
