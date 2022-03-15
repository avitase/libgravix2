#include "api.h"
#include <assert.h>
#include <math.h>

static double dist(double xyz[3]) {
    return acos(cos(lat(xyz[2])) * cos(lon(xyz[0], xyz[1])));
}

int main(int argc, char **argv) {
    const int N = 10;
    const double H = 1e-3;
    const double THRESHOLD = 1e-10;

    const double r = 10. / 180. * M_PI;
    const double v = v_scrcl(r);

    PlanetsHandle p = new_planets(1);
    int rc = set_planet(p, 0, 0., 0.);
    assert(rc == 0);

    TrajectoryBatch trj = new_missiles(1);
    struct Trajectory *m = get_trajectory(trj, 0);
    rc = init_missile(m, r, 0., v, 0., 1.);
    assert(rc == 0);

    int premature = 0;
    for (int i = 0; i < N; i++) {
        unsigned n = propagate_missile(m, p, H, &premature);
        assert(n == TRAJECTORY_SIZE);
        assert(premature == 0);

        for (int j = 0; j < n; j++) {
            const double dr = dist(m->x[j]) - r;
            const double dv = m->v[j][3] - v;
            assert(fabs(dr) < THRESHOLD);
            assert(fabs(dv) < THRESHOLD);
        }
    }

    delete_missiles(trj);
    delete_planets(p);

    return 0;
}
