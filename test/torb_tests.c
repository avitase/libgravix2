#include "api.h"
#include <assert.h>
#include <math.h>

static void test(double v, double psi) {
    const double H = 1e-3;

    PlanetsHandle p = new_planets(1);
    int rc = set_planet(p, 0, 0., 0.);
    assert(rc == 0);

    TrajectoryBatch trj = new_missiles(1);
    struct Trajectory *m = get_trajectory(trj, 0);
    rc = launch_missile(m, p, 0, v, psi);
    assert(rc == 0);

    int premature = 0;
    unsigned n = 0;
    while (premature == 0) {
        n += propagate_missile(m, p, H, &premature);
    }

    const double n_exp = orb_period(v, H);
    assert(fabs(n - n_exp) < 1);

    delete_missiles(trj);
    delete_planets(p);
}

int main(int argc, char **argv) {
    const double v = v_esc();
    test(.5 * v, 0.);
    test(0.99 * v, 90.);
    test(1.01 * v, -45.);
    test(2. * v, 1337.);
}
