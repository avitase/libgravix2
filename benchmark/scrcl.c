#include "api.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static double dist(double xyz[3]) {
    return acos(cos(lat(xyz[2])) * cos(lon(xyz[0], xyz[1])));
}

int main(int argc, char **argv) {
    const int N = (argc == 2) ? atoi(argv[1]) : 1000;
    const double H = 1e-6;
    const double r = .2;
    const double v = v_scrcl(r);

    PlanetsHandle p = new_planets(1);
    set_planet(p, 0, 0., 0.);

    TrajectoryBatch trj = new_missiles(1);
    struct Trajectory *m = get_trajectory(trj, 0);
    init_missile(m, r, 0., v, 0., 1.);

    int premature = 0;
    unsigned n = 0;
    for (int i = 0; i < N; i++) {
        n = propagate_missile(m, p, H, &premature);
    }

    const double dr = fabs(dist(m->x[n - 1]) - r);
    const double dv = fabs(m->v[n - 1][3] - v);
    printf("%d %f %f\n", premature, dr, dv);

    delete_missiles(trj);
    delete_planets(p);

    return 0;
}
