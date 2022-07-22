#include "api.h"
#include <assert.h>
#include <math.h>

int main(int argc, char **argv) {
    const double THRESHOLD = 1e-10;

    GrvxPlanetsHandle planets = grvx_new_planets(1);
    int rc = grvx_set_planet(planets, 0, 0., 0.);
    assert(rc == 0);

    GrvxTrajectoryBatch missiles = grvx_new_missiles(1);
    struct GrvxTrajectory *m = grvx_get_trajectory(missiles, 0);

    rc = grvx_launch_missile(m, planets, 0, 0., 0.);
    assert(rc == 0);

    const double x0 = m->x[GRVX_TRAJECTORY_SIZE - 1][0];
    const double y0 = m->x[GRVX_TRAJECTORY_SIZE - 1][1];
    const double z0 = m->x[GRVX_TRAJECTORY_SIZE - 1][2];
    const double vx0 = m->v[GRVX_TRAJECTORY_SIZE - 1][0];
    const double vy0 = m->v[GRVX_TRAJECTORY_SIZE - 1][1];
    const double vz0 = m->v[GRVX_TRAJECTORY_SIZE - 1][2];

    assert(fabs(m->x[0][0] - x0) < THRESHOLD);
    assert(fabs(m->x[0][1] - y0) < THRESHOLD);
    assert(fabs(m->x[0][2] - z0) < THRESHOLD);
    assert(fabs(m->v[0][0] - vx0) < THRESHOLD);
    assert(fabs(m->v[0][1] - vy0) < THRESHOLD);
    assert(fabs(m->v[0][2] - vz0) < THRESHOLD);

    int premature;
    unsigned n = grvx_propagate_missile(m, planets, 1e-4, &premature);
    assert(n > 0);
    assert(premature == 1);

    grvx_delete_missiles(missiles);
    grvx_delete_planets(planets);

    return 0;
}
