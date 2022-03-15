#include "helpers.h"
#include "api.h"
#include "linalg.h"
#include <math.h>

double lat(double z) {
    return asin(z);
}

double lon(double x, double y) {
    return atan2(x, y);
}

double vlat(double vx, double vy, double vz, double lat, double lon) {
    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    struct Vec3D v = {vx, vy, vz};
    struct Vec3D e_lat = {-sin_lat * sin_lon, -sin_lat * cos_lon, cos_lat};

    return dot(v, e_lat);
}

double vlon(double vx, double vy, double vz, double lon) {
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    struct Vec3D v = {vx, vy, vz};
    struct Vec3D e_lon = {cos_lon, -sin_lon};

    /*
     * WARNING: this is \f$\dot\lambda \, \cos\phi\f$
     */
    return dot(v, e_lon);
}

double sinc(double x) {
    return x != 0. ? sin(x) / x : 1.;
}
