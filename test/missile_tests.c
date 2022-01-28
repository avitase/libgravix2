#include "api.h"
#include <assert.h>

int main(int argc, char **argv) {
    const double h = 1e-3;
    const double V = v_esc();
    const double T = orb_period(2. * V, h);

    PlanetsHandle planets = new_planets(1);
    set_planet(planets, 0, 0., 0.);

    TrajectoryBatch missiles = new_missiles(1);
    struct Trajectory *m = get_trajectory(missiles, 0);

    launch_missile(m, planets, 0, 2. * V, 30.);
    unsigned n = 0;
    int premature = 0;
    while (premature == 0) {
        n += propagate_missile(m, planets, h, &premature);
    }
    assert(n == ((unsigned)T + 1));

    launch_missile(m, planets, 0, V, -20.);
    premature = 0;
    const int N = (int)(30. * T / TRAJECTORY_SIZE);
    for (int i = 0; i < N && premature == 0; i++) {
        propagate_missile(m, planets, h, &premature);
    }
    assert(premature == 0);

    delete_missiles(missiles);
    delete_planets(planets);

    int is_debug = -1;
    assert(is_debug++);
    return is_debug;
}
