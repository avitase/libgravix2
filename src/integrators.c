#include "integrators.h"

#include <assert.h>
#include <math.h>

#include "config.h"
#include "helpers.h"
#include "pot.h"

#if GRVX_COMPOSITION_P2S1 == GRVX_COMPOSITION_ID
// Vanilla Strang splitting
static const double GAMMA[] = {
    1.0,
};
#elif GRVX_COMPOSITION_P4S3 == GRVX_COMPOSITION_ID
// Tripple Jump
static const double GAMMA[] = {
    +1.35120719195965763405, // 1, 3
    -1.70241438391931526810, // 2
    +1.35120719195965763405, // 1, 3
};
#elif GRVX_COMPOSITION_P4S5 == GRVX_COMPOSITION_ID
// Suzuki's Fractal, DOI:10.1016/0375-9601(90)90962-N
static const double GAMMA[] = {
    +0.41449077179437573714, // 1, 2, 4, 5
    +0.41449077179437573714, // 1, 2, 4, 5
    -0.65796308717750294857, // 3
    +0.41449077179437573714, // 1, 2, 4, 5
    +0.41449077179437573714, // 1, 2, 4, 5
};
#elif GRVX_COMPOSITION_P6S9 == GRVX_COMPOSITION_ID
// Kahan & Li (1997), DOI:10.1090/S0025-5718-97-00873-9
static const double GAMMA[] = {
    +0.39216144400731413928,  // 1, 9
    +0.33259913678935943860,  // 2, 8
    -0.70624617255763935981,  // 3, 7
    +0.082213596293550800230, // 4, 6
    +0.79854399093482996340,  // 5
    +0.082213596293550800230, // 4, 6
    -0.70624617255763935981,  // 3, 7
    +0.33259913678935943860,  // 2, 8
    +0.39216144400731413928,  // 1, 9
};
#elif GRVX_COMPOSITION_P8S15 == GRVX_COMPOSITION_ID
// Suzuki & Umeno (1993), DOI:10.1007/978-3-642-78448-4_7
static const double GAMMA[] = {
    +0.74167036435061295345, // 1, 15
    -0.40910082580003159400, // 2, 14
    +0.19075471029623837995, // 3, 13
    -0.57386247111608226666, // 4, 12
    +0.29906418130365592384, // 5, 11
    +0.33462491824529818378, // 6, 10
    +0.31529309239676659663, // 7, 9
    -0.79688793935291635402, // 8
    +0.31529309239676659663, // 7, 9
    +0.33462491824529818378, // 6, 10
    +0.29906418130365592384, // 5, 11
    -0.57386247111608226666, // 4, 12
    +0.19075471029623837995, // 3, 13
    -0.40910082580003159400, // 2, 14
    +0.74167036435061295345, // 1, 15
};
#endif

static void strang1(struct GrvxQP *qp, struct GrvxQP *e, double h)
{
    const double p2 = grvx_dot(qp->p, qp->p);
    const double ph = sqrt(p2) * h;

    const double h_sinc_ph = h * grvx_sinc(ph);
    const double cos_ph_minus_one = -2. * pow(sin(ph / 2.), 2);

    struct GrvxQP d = {
        .q.x = qp->q.x * cos_ph_minus_one + qp->p.x * h_sinc_ph,
        .q.y = qp->q.y * cos_ph_minus_one + qp->p.y * h_sinc_ph,
        .q.z = qp->q.z * cos_ph_minus_one + qp->p.z * h_sinc_ph,
        .p.x = qp->p.x * cos_ph_minus_one - qp->q.x * p2 * h_sinc_ph,
        .p.y = qp->p.y * cos_ph_minus_one - qp->q.y * p2 * h_sinc_ph,
        .p.z = qp->p.z * cos_ph_minus_one - qp->q.z * p2 * h_sinc_ph,
    };

    e->q.x += d.q.x;
    e->q.y += d.q.y;
    e->q.z += d.q.z;
    e->p.x += d.p.x;
    e->p.y += d.p.y;
    e->p.z += d.p.z;

    struct GrvxQP qp2 = {
        .q.x = qp->q.x + e->q.x,
        .q.y = qp->q.y + e->q.y,
        .q.z = qp->q.z + e->q.z,
        .p.x = qp->p.x + e->p.x,
        .p.y = qp->p.y + e->p.y,
        .p.z = qp->p.z + e->p.z,
    };

    e->q.x += qp->q.x - qp2.q.x;
    e->q.y += qp->q.y - qp2.q.y;
    e->q.z += qp->q.z - qp2.q.z;
    e->p.x += qp->p.x - qp2.p.x;
    e->p.y += qp->p.y - qp2.p.y;
    e->p.z += qp->p.z - qp2.p.z;

    *qp = qp2;
}

static void strang2(struct GrvxQP *qp,
                    struct GrvxQP *e,
                    double h,
                    const struct GrvxPlanets *planets)
{
    struct GrvxVec3D v = qp->q;
    grvx_gradV(&v, planets);
    const double q_dot_gradV = grvx_dot(qp->q, v);

    struct GrvxVec3D dp = {
        (q_dot_gradV * qp->q.x - v.x) * h,
        (q_dot_gradV * qp->q.y - v.y) * h,
        (q_dot_gradV * qp->q.z - v.z) * h,
    };

    e->p.x += dp.x;
    e->p.y += dp.y;
    e->p.z += dp.z;

    struct GrvxQP qp2 = {
        .q.x = qp->q.x,
        .q.y = qp->q.y,
        .q.z = qp->q.z,
        .p.x = qp->p.x + e->p.x,
        .p.y = qp->p.y + e->p.y,
        .p.z = qp->p.z + e->p.z,
    };

    e->p.x += qp->p.x - qp2.p.x;
    e->p.y += qp->p.y - qp2.p.y;
    e->p.z += qp->p.z - qp2.p.z;

    *qp = qp2;
}

void grvx_integration_step(struct GrvxQP *qp,
                           struct GrvxQP *e,
                           double h,
                           const struct GrvxPlanets *planets)
{
    strang1(qp, e, GAMMA[0] * h / 2.);
    for (unsigned i = 0; i < GRVX_COMPOSITION_STAGES; i++) {
        const double g2 = GAMMA[i];
        const double g1 =
            g2 + (i + 1 < GRVX_COMPOSITION_STAGES ? GAMMA[i + 1] : 0.);

        strang2(qp, e, g2 * h, planets);
        strang1(qp, e, g1 * h / 2.);
    }
}

unsigned grvx_integration_loop(struct GrvxQP *qp,
                               double h,
                               unsigned n,
                               const struct GrvxPlanets *planets)
{
    double mdist = -1.;
    const double threshold = cos(GRVX_MIN_DIST);

    struct GrvxQP e = {{0., 0., 0.}, {0., 0., 0.}};
    for (; n > 0 && mdist < threshold; n--) {
        grvx_integration_step(qp, &e, h, planets);
        mdist = grvx_min_dist(&qp->q, planets);
        assert(fabs(mdist) <= 1);
    }

    const double q_norm = 1. / grvx_mag(qp->q);
    qp->q.x *= q_norm;
    qp->q.y *= q_norm;
    qp->q.z *= q_norm;

    const double error = grvx_dot(qp->q, qp->p);
    qp->p.x -= error * qp->q.x;
    qp->p.y -= error * qp->q.y;
    qp->p.z -= error * qp->q.z;

    return n;
}
