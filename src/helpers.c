#include "helpers.h"

#include <math.h>

#include "api.h"
#include "linalg.h"

double grvx_lat(double z)
{
    return asin(z);
}

double grvx_lon(double x, double y)
{
    return atan2(x, y);
}

double grvx_vlat(double vx, double vy, double vz, double lat, double lon)
{
    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    struct GrvxVec3D v = {vx, vy, vz};
    struct GrvxVec3D e_lat = {-sin_lat * sin_lon, -sin_lat * cos_lon, cos_lat};

    return grvx_dot(v, e_lat);
}

double grvx_vlon(double vx, double vy, double vz, double lon)
{
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    struct GrvxVec3D v = {vx, vy, vz};
    struct GrvxVec3D e_lon = {cos_lon, -sin_lon, 0.};

    /*
     * WARNING: this is \f$\dot\lambda \, \cos\phi\f$
     */
    return grvx_dot(v, e_lon);
}

double grvx_sinc(double x)
{
    return fabs(x) > 0. ? sin(x) / x : 1.;
}
