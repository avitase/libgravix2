#include "integrators.h"
#include "config.h"
#include "helpers.h"
#include "pot.h"
#include <assert.h>
#include <math.h>

/*!
 * \brief Vanilla Strang splitting
 */
const double P2GAMMA_p2s1[] = {
    1.0,
};

/*!
 * \brief Tripple Jump
 *
 * 4th-order symmetric composition method with 3 stages.
 */
const double P2GAMMA_p4s3[] = {
    1.35120719195965763405,  // 1, 3
    -1.70241438391931526810, // 2
    1.35120719195965763405,  // 1, 3
};

/*!
 * \brief Suzuki's Fractal
 *
 * 4th-order symmetric composition method with 5 stages.
 * See <a
 * href="https://doi.org/10.1016/0375-9601(90)90962-N">DOI:10.1016/0375-9601(90)90962-N</a>
 * for more information.
 */
const double P2GAMMA_p4s5[] = {
    0.41449077179437573714,  // 1, 2, 4, 5
    0.41449077179437573714,  // 1, 2, 4, 5
    -0.65796308717750294857, // 3
    0.41449077179437573714,  // 1, 2, 4, 5
    0.41449077179437573714,  // 1, 2, 4, 5
};

/*!
 * \brief Kahan & Li (1997)
 *
 * 6th-order symmetric composition method with 9 stages.
 * See <a
 * href="https://doi.org/10.1090/S0025-5718-97-00873-9">DOI:10.1090/S0025-5718-97-00873-9</a>
 * for more information.
 */
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

/*!
 * \brief Suzuki & Umeno (1993)
 *
 * 8th-order symmetric composition method with 15 stages.
 * See <a
 * href="https://doi.org/10.1007/978-3-642-78448-4_7">DOI:10.1007/978-3-642-78448-4_7</a>
 * for more information.
 */
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
    const double sin_ph = sin(qp->p_abs * h);
    const double cos_ph = cos(qp->p_abs * h);
    struct QP qp2 = {
        .q.x = qp->q.x * cos_ph + qp->p.x * sin_ph,
        .q.y = qp->q.y * cos_ph + qp->p.y * sin_ph,
        .q.z = qp->q.z * cos_ph + qp->p.z * sin_ph,
        .p.x = -qp->q.x * sin_ph + qp->p.x * cos_ph,
        .p.y = -qp->q.y * sin_ph + qp->p.y * cos_ph,
        .p.z = -qp->q.z * sin_ph + qp->p.z * cos_ph,
        .p_abs = qp->p_abs,
    };
    *qp = qp2;
}

static void strang2(struct QP *qp, double h, const struct Planets *planets) {
    struct Vec3D v = qp->q;
    gradV(&v, planets);
    const double q_dot_gradV = dot(qp->q, v);

    struct Vec3D ah = {
        v.x = (q_dot_gradV * qp->q.x - v.x) * h,
        v.y = (q_dot_gradV * qp->q.y - v.y) * h,
        v.z = (q_dot_gradV * qp->q.z - v.z) * h,
    };

    v.x = qp->p_abs * qp->p.x + ah.x;
    v.y = qp->p_abs * qp->p.y + ah.y;
    v.z = qp->p_abs * qp->p.z + ah.z;
    qp->p_abs = mag(v);

    const double p_min = P_MIN;
    const int too_small = (qp->p_abs < p_min);
    qp->p.x = too_small ? qp->p.x : v.x / qp->p_abs;
    qp->p.y = too_small ? qp->p.y : v.y / qp->p_abs;
    qp->p.z = too_small ? qp->p.z : v.z / qp->p_abs;
    qp->p_abs = too_small ? 0. : qp->p_abs;
}

void integration_step(struct QP *qp, double h, const struct Planets *planets) {
    strang1(qp, GAMMA[0] * h / 2.);
    for (unsigned i = 0; i < N_STAGES; i++) {
        const double g2 = GAMMA[i];
        const double g1 = g2 + (i + 1 < N_STAGES ? GAMMA[i + 1] : 0.);

        strang2(qp, g2 * h, planets);
        strang1(qp, g1 * h / 2.);
    }

    // TODO: necessary?
    const double q_norm = 1. / mag(qp->q);
    qp->q.x *= q_norm;
    qp->q.y *= q_norm;
    qp->q.z *= q_norm;

    // TODO: necessary?
    // ... probably not since strang2 normalizes p
    const double p_norm = 1. / mag(qp->p);
    qp->p.x *= p_norm;
    qp->p.y *= p_norm;
    qp->p.z *= p_norm;
}

unsigned integration_loop(struct QP *qp,
                          double h,
                          unsigned n,
                          const struct Planets *planets) {
    double mdist = -1.;
    const double threshold = cos(THRESHOLD);

    for (; n > 0 && mdist < threshold; n--) {
        integration_step(qp, h, planets);
        mdist = min_dist(&qp->q, planets);
    }

    return n;
}
