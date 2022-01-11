#include "linalg.h"

double dot(struct Vec3D a, struct Vec3D b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
