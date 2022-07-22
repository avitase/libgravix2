#include "api.h"
#include <assert.h>
#include <math.h>

int main(int argc, char **argv) {
    const double H = 1e-3;
    const double THRESHOLD = 1e-10;

    const double V0 = 2 * grvx_v_esc();
    const unsigned N = 50;
    assert(N < GRVX_TRAJECTORY_SIZE);
    assert(N < grvx_orb_period(V0, H));

    GrvxPlanetsHandle p = grvx_new_planets(1);

    int rc = grvx_set_planet(p, 0, 0., 0.);
    assert(rc == 0);

    GrvxTrajectoryBatch trj = grvx_new_missiles(2);

    struct GrvxTrajectory *m1 = grvx_get_trajectory(trj, 0);
    struct GrvxTrajectory *m2 = grvx_get_trajectory(trj, 1);

    rc = grvx_launch_missile(m1, p, 0, V0, -M_PI / 2);
    assert(rc == 0);

    int premature;
    unsigned n = grvx_propagate_missile(m1, p, H, &premature);
    assert(n >= N);

    /*
     *  \
     *  o  x  x  x  x  x  x  x
     *  /  0  1  2  3  4  5  6
     *  ^-- planet     ^-- N = 5
     */
    double *x = m1->x[N - 1];
    double *v = m1->v[N - 1];

    const double lat2 = grvx_lat(x[2]);
    const double lon2 = grvx_lon(x[0], x[1]);
    const double vlat2 = grvx_vlat(v[0], v[1], v[2], lat2, lon2);
    const double vlon2 = grvx_vlon(v[0], v[1], v[2], lon2);
    const double v2_mag = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    rc = grvx_init_missile(m2, lat2, lon2, v2_mag, -vlat2, -vlon2);
    assert(rc == 0);

    unsigned n2 = grvx_propagate_missile(m2, p, H, &premature);
    assert(premature != 0);

    // integration stops *after* the missile has passed the rim
    assert(n2 == N || n2 == N + 1);

    for (unsigned i = 0; i < N - 2; i++) {
        unsigned j = N - i - 2; // init is not part of trajectory
        assert(fabs(m1->x[j][0] - m2->x[i][0]) < THRESHOLD);
        assert(fabs(m1->x[j][1] - m2->x[i][1]) < THRESHOLD);
        assert(fabs(m1->x[j][2] - m2->x[i][2]) < THRESHOLD);
        assert(fabs(m1->v[j][0] + m2->v[i][0]) < THRESHOLD);
        assert(fabs(m1->v[j][1] + m2->v[i][1]) < THRESHOLD);
        assert(fabs(m1->v[j][2] + m2->v[i][2]) < THRESHOLD);
    }

    grvx_delete_missiles(trj);
    grvx_delete_planets(p);

    return 0;
}
