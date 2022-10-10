#include "libgravix2/api.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static double dist(double xyz[3])
{
    return acos(cos(grvx_lat(xyz[2])) * cos(grvx_lon(xyz[0], xyz[1])));
}

int main(int argc, char **argv)
{
    const long N = (argc == 2) ? strtol(argv[1], NULL, 10) : 1000;
    const double H = 1e-6;
    const double r = .2;
    const double v = grvx_v_scrcl(r);

    GrvxPlanetsHandle p = grvx_new_planets(1);
    grvx_set_planet(p, 0, 0., 0.);

    GrvxTrajectoryBatch trj = grvx_new_missiles(1);
    struct GrvxTrajectory *m = grvx_get_trajectory(trj, 0);
    grvx_init_missile(m, r, 0., v, 0., 1.);

    int premature = 0;
    unsigned n = 0;
    for (int i = 0; i < N; i++) {
        n = grvx_propagate_missile(m, p, H, &premature);
    }

    const double dr = fabs(dist(m->x[n - 1]) - r);

    double vx = m->v[n - 1][0];
    double vy = m->v[n - 1][1];
    double vz = m->v[n - 1][2];
    const double dv = sqrt(pow(vx, 2) + pow(vy, 2) + pow(vz, 2)) - v;
    printf("%d %g %g\n", premature, dr, dv);

    grvx_delete_missiles(trj);
    grvx_delete_planets(p);

    return 0;
}
