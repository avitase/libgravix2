#include "libgravix2/pot.h"

#include <math.h>
#include <stddef.h>

#include "libgravix2/api.h"
#include "libgravix2/config.h"
#include "libgravix2/linalg.h"
#include "libgravix2/planet.h"

#if GRVX_POT_TYPE == GRVX_POT_TYPE_3D
#include "libgravix2/constants.h"
#include "libgravix2/helpers.h"
#endif

#if GRVX_POT_TYPE == GRVX_POT_TYPE_3D

static double pot3D_approx(double x)
{
    const double TWO_PI = 2. * M_PI;

    // TODO: use lookup table and cubic hermite spline
    double acc = 0.;

    // accumulate contributions starting with the smallest one
    for (unsigned i = 0; i < GRVX_N_POT; i++) {
        acc += 1. / (TWO_PI * i + x) + 1. / (TWO_PI * (i + 1) - x) -
               4. / (TWO_PI * (2. * i + 1.));
    }

    return acc / (2. * TWO_PI);
}

static double f3D_approx(double x)
{
    // TODO: use lookup table and cubic hermite spline
    double acc = 0.;

    // accumulate contributions starting with the smallest one
    for (unsigned i = 0; i < GRVX_N_POT; i++) {
        double k = (double)(2 * (GRVX_N_POT - 1 - i) + 1);
        acc += k / pow(M_PI * M_PI * k * k - x * x, 2);
    }

    return -acc / grvx_sinc(x);
}

#endif

void grvx_gradV(struct GrvxVec3D *x, const struct GrvxPlanets *planets)
{
    struct GrvxVec3D acc = {0., 0., 0.};

    const ptrdiff_t N = (ptrdiff_t)planets->n;
    for (ptrdiff_t i = 0; i < N; i++) {
        struct GrvxVec3D planet = {
            planets->data[3 * i],
            planets->data[3 * i + 1],
            planets->data[3 * i + 2],
        };
        const double d = grvx_dot(*x, planet);

#if GRVX_POT_TYPE == GRVX_POT_TYPE_2D
        const double s = -1. / (1. - d);
#elif GRVX_POT_TYPE == GRVX_POT_TYPE_3D
        const double s = f3D_approx(acos(d) - M_PI);
#endif

        acc.x += s * planet.x;
        acc.y += s * planet.y;
        acc.z += s * planet.z;
    }

    *x = acc;
}

double grvx_min_dist(const struct GrvxVec3D *x,
                     const struct GrvxPlanets *planets)
{
    double mdist = -1.;

    const ptrdiff_t N = (ptrdiff_t)planets->n;
    for (ptrdiff_t i = 0; i < N; i++) {
        struct GrvxVec3D planet = {
            planets->data[3 * i],
            planets->data[3 * i + 1],
            planets->data[3 * i + 2],
        };
        const double d = grvx_dot(*x, planet);
        mdist = d > mdist ? d : mdist;
    }

    return mdist;
}

double grvx_v_esc(void)
{
#if GRVX_POT_TYPE == GRVX_POT_TYPE_2D
    const double pot = -2. * log(sin(GRVX_MIN_DIST / 2.));
#elif GRVX_POT_TYPE == GRVX_POT_TYPE_3D
    const double pot = pot3D_approx(GRVX_MIN_DIST);
#endif

    return sqrt(2. * pot);
}

double grvx_v_scrcl(double r)
{
#if GRVX_POT_TYPE == GRVX_POT_TYPE_2D
    return sqrt((1. + cos(r)) / fabs(cos(r)));
#elif GRVX_POT_TYPE == GRVX_POT_TYPE_3D
    return sin(r) * sqrt(-f3D_approx(r - M_PI) / fabs(cos(r)));
#endif
}
