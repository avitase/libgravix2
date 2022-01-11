#include "pot.h"
#include "config.h"
#include "linalg.h"
#include "planet.h"

#if POT_TYPE == POT_TYPE_3D
#include "helpers.h"
#include <math.h>
#endif

#if POT_TYPE == POT_TYPE_3D
static double v3D_approx(double x) {
    // TODO: use lookup table and cubic hermite spline
    double acc = 0.;

    // accumulate contributions starting with the smallest one
    for (unsigned i = 0; i < N_POT; i++) {
        double k = (double)(2 * (N_POT - 1 - i) + 1);
        acc += k / pow(M_PI * M_PI * k * k - x * x, 2);
    }

    return -acc / sinc(x);
}
#endif

double gradV(struct Vec3D *x, const struct Planets *planets) {
    struct Vec3D acc = {0., 0., 0.};
    double mdist = -1.;

    for (unsigned i = 0; i < planets->n; i++) {
        struct Vec3D planet = {
            planets->data[3 * i],
            planets->data[3 * i + 1],
            planets->data[3 * i + 2],
        };
        const double d = dot(*x, planet);
        mdist = d > mdist ? d : mdist;

#if POT_TYPE == POT_TYPE_2D
        const double s = -1. / (1. - d);
#elif POT_TYPE == POT_TYPE_3D
        const double s = v3D_approx(acos(d) - M_PI);
#endif

        acc.x += s * planet.x;
        acc.y += s * planet.y;
        acc.z += s * planet.z;
    }

    *x = acc;

    return mdist;
}
