#include "libgravix2/linalg.h"

#include <math.h>

double grvx_dot(struct GrvxVec3D a, struct GrvxVec3D b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double grvx_mag(struct GrvxVec3D v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
