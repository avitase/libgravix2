#include "pot.h"
#include "api.h"
#include "config.h"
#include "linalg.h"
#include "planet.h"
#include <math.h>

#if POT_TYPE == POT_TYPE_3D
#include "helpers.h"
#endif

#if POT_TYPE == POT_TYPE_3D
static double pot3D_approx(double x) {
    const double TWO_PI = 2. * M_PI;

    // TODO: use lookup table and cubic hermite spline
    double acc = 0.;

    // accumulate contributions starting with the smallest one
    for (unsigned i = 0; i < N_POT; i++) {
        acc += 1. / (TWO_PI * i + x) + 1. / (TWO_PI * (i + 1) - x) -
               4. / (TWO_PI * (2. * i + 1.));
    }

    return acc / (2. * TWO_PI);
}

static double f3D_approx(double x) {
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
        // assert(fabs(d) <= 1.);
        mdist = d > mdist ? d : mdist;

#if POT_TYPE == POT_TYPE_2D
        const double s = -1. / (1. - d);
#elif POT_TYPE == POT_TYPE_3D
        const double s = f3D_approx(acos(d) - M_PI);
#endif

        acc.x += s * planet.x;
        acc.y += s * planet.y;
        acc.z += s * planet.z;
    }

    *x = acc;

    return mdist;
}

double v_esc(void) {
    const double DEG2RAD = M_PI / 180.;
    const double RAD2DEG = 180. / M_PI;

    const double x = MIN_DIST * DEG2RAD;

#if POT_TYPE == POT_TYPE_2D
    const double pot = -2. * log(sin(x / 2.));
#elif POT_TYPE == POT_TYPE_3D
    const double pot = pot3D_approx(x);
#endif

    return sqrt(2. * pot) * RAD2DEG;
}

double v_scrcl(double r) {
    const double DEG2RAD = M_PI / 180.;
    const double RAD2DEG = 180. / M_PI;

    r *= DEG2RAD;

#if POT_TYPE == POT_TYPE_2D
    const double v = sqrt((1. + cos(r)) / fabs(cos(r)));
#elif POT_TYPE == POT_TYPE_3D
    const double v = sin(r) * sqrt(-f3D_approx(r - M_PI) / fabs(cos(r)));
#endif

    return v * RAD2DEG;
}
