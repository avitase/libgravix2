#include "api.h"
#include <assert.h>
#include <math.h>

static double dist(double xyz[3]) {
    return acos(cos(grvx_lat(xyz[2])) * cos(grvx_lon(xyz[0], xyz[1])));
}

int main(int argc, char **argv) {
    const int N = 10;
    const double H = 1e-3;
    const double THRESHOLD = 1e-10;

    const double r = 10. / 180. * M_PI;
    const double v = grvx_v_scrcl(r);

    GrvxPlanetsHandle p = grvx_new_planets(1);
    int rc = grvx_set_planet(p, 0, 0., 0.);
    assert(rc == 0);

    GrvxTrajectoryBatch trj = grvx_new_missiles(1);
    struct GrvxTrajectory *m = grvx_get_trajectory(trj, 0);
    rc = grvx_init_missile(m, r, 0., v, 0., 1.);
    assert(rc == 0);

    int premature = 0;
    for (int i = 0; i < N; i++) {
        unsigned n = grvx_propagate_missile(m, p, H, &premature);
        assert(n == GRVX_TRAJECTORY_SIZE);
        assert(premature == 0);

        for (int j = 0; j < n; j++) {
            const double dr = dist(m->x[j]) - r;

            double vx = m->v[j][0];
            double vy = m->v[j][1];
            double vz = m->v[j][2];
            const double dv = sqrt(pow(vx, 2) + pow(vy, 2) + pow(vz, 2)) - v;
            assert(fabs(dr) < THRESHOLD);
            assert(fabs(dv) < THRESHOLD);
        }
    }

    grvx_delete_missiles(trj);
    grvx_delete_planets(p);

    return 0;
}
