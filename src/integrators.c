#include "integrators.h"
#include "config.h"
#include "helpers.h"
#include "pot.h"
#include <math.h>

const double P2GAMMA_p2s1[] = {
    1.0,
};

const double P2GAMMA_p6s9[] = {
    0.39216144400731413928,  // 1, 9
    0.33259913678935943860,  // 2, 8
    -0.70624617255763935981, // 3, 7
    0.082213596293550800230, // 4, 6
    0.79854399093482996340,  // 5
    0.082213596293550800230, // 4, 6
    -0.70624617255763935981, // 3, 7
    0.33259913678935943860,  // 2, 8
    0.39216144400731413928,  // 1, 9
};

const double P2GAMMA_p8s15[] = {
    0.74167036435061295345,  // 1, 15
    -0.40910082580003159400, // 2, 14
    0.19075471029623837995,  // 3, 13
    -0.57386247111608226666, // 4, 12
    0.29906418130365592384,  // 5, 11
    0.33462491824529818378,  // 6, 10
    0.31529309239676659663,  // 7, 9
    -0.79688793935291635402, // 8
    0.31529309239676659663,  // 7, 9
    0.33462491824529818378,  // 6, 10
    0.29906418130365592384,  // 5, 11
    -0.57386247111608226666, // 4, 12
    0.19075471029623837995,  // 3, 13
    -0.40910082580003159400, // 2, 14
    0.74167036435061295345,  // 1, 15
};

static const double *const GAMMA = COMPOSITION_SCHEME;
static const unsigned N_STAGES =
    sizeof(COMPOSITION_SCHEME) / sizeof(*COMPOSITION_SCHEME);

static const double THRESHOLD = MIN_DIST / 180. * M_PI;

static void strang1(struct QP *qp, double h) {
    const double p = sqrt(dot(qp->p, qp->p));
    const double s = sin(p * h);
    const double c = cos(p * h);

    /*
     * TODO: test if approximation with square root is more efficient
     */
    const double sinc_ph = sinc(p * h);
    struct QP qp2 = {.q.x = qp->p.x * h * sinc_ph + qp->q.x * c,
                     .q.y = qp->p.y * h * sinc_ph + qp->q.y * c,
                     .q.z = qp->p.z * h * sinc_ph + qp->q.z * c,
                     .p.x = qp->p.x * c - qp->q.x * p * s,
                     .p.y = qp->p.y * c - qp->q.y * p * s,
                     .p.z = qp->p.z * c - qp->q.z * p * s};
    *qp = qp2;
}

static double strang2(struct QP *qp, double h, const struct Planets *planets) {
    struct Vec3D v = qp->q;
    const double mdist = gradV(&v, planets);
    const double q_dot_gradV = dot(qp->q, v);

    qp->p.x += (q_dot_gradV * qp->q.x - v.x) * h;
    qp->p.y += (q_dot_gradV * qp->q.y - v.y) * h;
    qp->p.z += (q_dot_gradV * qp->q.z - v.z) * h;

    return mdist;
}

double
integration_step(struct QP *qp, double h, const struct Planets *planets) {
    double mdist = -1.;

    strang1(qp, GAMMA[0] * h / 2.);
    for (unsigned i = 0; i < N_STAGES; i++) {
        const double g2 = GAMMA[i];
        const double g1 = g2 + (i + 1 < N_STAGES ? GAMMA[i + 1] : 0.);

        mdist = strang2(qp, g2 * h, planets);
        strang1(qp, g1 * h / 2.);
    }

    return mdist;
}

unsigned integration_loop(struct QP *qp,
                          double h,
                          unsigned n,
                          const struct Planets *planets) {
    double mdist = -1.;
    const double threshold = cos(THRESHOLD);

    for (; n > 0 && mdist < threshold; n--) {
        mdist = integration_step(qp, h, planets);
    }

    return n;
}
