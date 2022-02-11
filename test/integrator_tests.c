#include "api.h"
#include <assert.h>
#include <math.h>

int main(int argc, char **argv) {
    const double threshold = 1e-10;

    PlanetsHandle planets = new_planets(1);
    int rc = set_planet(planets, 0, 0., 0.);
    assert(rc == 0);

    TrajectoryBatch missiles = new_missiles(1);
    struct Trajectory *m = get_trajectory(missiles, 0);

    rc = launch_missile(m, planets, 0, 0., 0.);
    assert(rc == 0);

    const double x0 = m->x[TRAJECTORY_SIZE - 1][0];
    const double y0 = m->x[TRAJECTORY_SIZE - 1][1];
    const double z0 = m->x[TRAJECTORY_SIZE - 1][2];
    const double vx0 = m->v[TRAJECTORY_SIZE - 1][0];
    const double vy0 = m->v[TRAJECTORY_SIZE - 1][1];
    const double vz0 = m->v[TRAJECTORY_SIZE - 1][2];

    assert(fabs(m->x[0][0] - x0) < threshold);
    assert(fabs(m->x[0][1] - y0) < threshold);
    assert(fabs(m->x[0][2] - z0) < threshold);
    assert(fabs(m->v[0][0] - vx0) < threshold);
    assert(fabs(m->v[0][1] - vy0) < threshold);
    assert(fabs(m->v[0][2] - vz0) < threshold);

    int premature;
    unsigned n = propagate_missile(m, planets, 1e-4, &premature);
    assert(n > 0);
    assert(premature == 1);

    delete_missiles(missiles);
    delete_planets(planets);

    return 0;
}
