#include "api.h"
#include <assert.h>
#include <math.h>

static void test(double v, double psi) {
    const double H = 1e-3;

    GrvxPlanetsHandle p = grvx_new_planets(1);
    int rc = grvx_set_planet(p, 0, 0., 0.);
    assert(rc == 0);

    GrvxTrajectoryBatch trj = grvx_new_missiles(1);
    struct GrvxTrajectory *m = grvx_get_trajectory(trj, 0);
    rc = grvx_launch_missile(m, p, 0, v, psi);
    assert(rc == 0);

    int premature = 0;
    unsigned n = 0;
    while (premature == 0) {
        n += grvx_propagate_missile(m, p, H, &premature);
    }

    const double n_exp = grvx_orb_period(v, H);
    assert(fabs(n - n_exp) < 1);

    grvx_delete_missiles(trj);
    grvx_delete_planets(p);
}

int main(int argc, char **argv) {
    const double v = grvx_v_esc();
    test(.5 * v, 0.);
    test(0.99 * v, M_PI / 2.);
    test(1.01 * v, -M_PI / 4.);
    test(2. * v, 1337.);
}
