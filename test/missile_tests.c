#include "api.h"
#include "linalg.h"
#include <assert.h>
#include <math.h>

int main(int argc, char **argv) {
    const double THRESHOLD = 1e-10;
    const double H = 1e-3;

    const double V = v_esc();
    assert(!isnan(V) && V > 0.);

    const double T = orb_period(2. * V, H);
    assert(!isnan(T) && T > 0.);

    PlanetsHandle planets = new_planets(1);
    set_planet(planets, 0, 0., 0.);

    TrajectoryBatch missiles = new_missiles(1);
    struct Trajectory *m = get_trajectory(missiles, 0);

    launch_missile(m, planets, 0, 2. * V, 0.);
    unsigned n_acc = 0;
    int premature = 0;
    while (premature == 0) {
        unsigned n = propagate_missile(m, planets, H, &premature);
        n_acc += n;

        for (unsigned i = 0; i < n; i++) {
            struct Vec3D q = {m->x[i][0], m->x[i][1], m->x[i][2]};
            struct Vec3D p = {m->v[i][0], m->v[i][1], m->v[i][2]};
            assert(fabs(dot(q, q) - 1.) < THRESHOLD);
            assert(fabs(dot(q, p)) < THRESHOLD);
        }
    }
    assert(n_acc == ((unsigned)T + 1));

    launch_missile(m, planets, 0, V, -1.5);
    premature = 0;
    const int N = (int)(30. * T / TRAJECTORY_SIZE);
    for (int i = 0; i < N && premature == 0; i++) {
        propagate_missile(m, planets, H, &premature);
    }
    assert(premature == 0);

    delete_missiles(missiles);
    delete_planets(planets);

    return 0;
}
