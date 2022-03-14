#include "api.h"
#include <assert.h>
#include <math.h>

static double dist_deg(double xyz[3]) {
    double lat = asin(xyz[2]);
    double lon = atan2(xyz[0], xyz[1]);
    return acos(cos(lat) * cos(lon)) * 180. / M_PI;
}

int main(int argc, char **argv) {
    const int N = 10;
    const double H = 1e-3;

    int rc;

    const double r_deg = 10.;
    const double v = v_scrcl(r_deg);

    PlanetsHandle p = new_planets(1);
    rc = set_planet(p, 0, 0., 0.);
    assert(rc == 0);

    TrajectoryBatch trj = new_missiles(1);
    struct Trajectory *m = get_trajectory(trj, 0);
    rc = init_missile(m, r_deg, 0., v, 0., 1.);
    assert(rc == 0);

    int premature = 0;
    for (int i = 0; i < N; i++) {
        unsigned n = propagate_missile(m, p, H, &premature);
        assert(n == TRAJECTORY_SIZE);
        assert(premature == 0);

        for (int j = 0; j < n; j++) {
            const double dr_deg = dist_deg(m->x[j]) - r_deg;
            const double dv = m->v[j][3] - v;
            assert(fabs(dr_deg) * M_PI / 180. < 1e-10);
            assert(fabs(dv) < 1e-10);
        }
    }

    delete_missiles(trj);
    delete_planets(p);

    return 0;
}
