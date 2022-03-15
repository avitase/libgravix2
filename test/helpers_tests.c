#include "api.h"
#include <assert.h>
#include <math.h>

int main(int argc, char **argv) {
    const double THRESHOLD = 1e-10;

    assert(fabs(lat(.3) - asin(.3)) < THRESHOLD);
    assert(fabs(lon(.2, .4)) - atan2(.2, .4) < THRESHOLD);

    const double vx = -.1 * (sin(.2) * sin(.3)) + .4 * cos(.2) * cos(.3);
    const double vy = -.1 * (sin(.2) * cos(.3)) - .4 * cos(.2) * sin(.3);
    const double vz = .1 * cos(.2);
    assert(fabs(vlat(vx, vy, vz, .2, .3) - vz / cos(.2)) < THRESHOLD);
    assert(fabs(vlon(vx, vy, vz, .3) - (vx * cos(.3) - vy * sin(.3))) <
           THRESHOLD);

    return 0;
}
