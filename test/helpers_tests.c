#include "api.h"
#include <assert.h>
#include <math.h>

int main(int argc, char **argv) {
    const double threshold = 1e-10;
    const double DEG2RAD = M_PI / 180.;
    const double RAD2DEG = 1. / DEG2RAD;

    assert(fabs(lat(.3) * DEG2RAD - asin(.3)) < threshold);
    assert(fabs(lon(.2, .4) * DEG2RAD) - atan2(.2, .4) < threshold);

    const double vx = -.1 * (sin(.2) * sin(.3)) + .4 * cos(.2) * cos(.3);
    const double vy = -.1 * (sin(.2) * cos(.3)) - .4 * cos(.2) * sin(.3);
    const double vz = .1 * cos(.2);
    assert(fabs(vlat(vx, vy, vz, .2 * RAD2DEG, .3 * RAD2DEG) -
                vz / cos(.2) * RAD2DEG) < threshold);
    assert(fabs(vlon(vx, vy, vz, .3 * RAD2DEG) -
                (vx * cos(.3) - vy * sin(.3)) * RAD2DEG) < threshold);

    return 0;
}
