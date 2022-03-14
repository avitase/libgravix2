#include "api.h"
#include <assert.h>
#include <math.h>

int main(int argc, char **argv) {
    const double H = 1e-3;
    const double THRESHOLD = 1e-10;

    const double V0 = 2 * v_esc();
    const unsigned N = 50;
    assert(N < TRAJECTORY_SIZE);
    assert(N < orb_period(V0, H));

    PlanetsHandle p = new_planets(1);

    int rc = set_planet(p, 0, 0., 0.);
    assert(rc == 0);

    TrajectoryBatch trj = new_missiles(2);

    struct Trajectory *m1 = get_trajectory(trj, 0);
    struct Trajectory *m2 = get_trajectory(trj, 1);

    rc = launch_missile(m1, p, 0, V0, -90.);
    assert(rc == 0);

    int premature;
    unsigned n = propagate_missile(m1, p, H, &premature);
    assert(n >= N);

    /*
     *  \
     *  o  x  x  x  x  x  x  x
     *  /  0  1  2  3  4  5  6
     *  ^-- planet     ^-- N = 5
     */
    double *x = m1->x[N - 1];
    double *v = m1->v[N - 1];

    const double lat2 = lat(x[2]);
    const double lon2 = lon(x[0], x[1]);
    const double vlat2 = vlat(v[0], v[1], v[2], lat2, lon2);
    const double vlon2 = vlon(v[0], v[1], v[2], lon2);
    rc = init_missile(m2, lat2, lon2, -v[3], vlat2, vlon2);
    assert(rc == 0);

    unsigned n2 = propagate_missile(m2, p, H, &premature);
    assert(premature != 0);

    // integration stops *after* the missile has passed the rim
    assert(n2 == N || n2 == N + 1);

    for (unsigned i = 0; i < N - 2; i++) {
        unsigned j = N - i - 2; // init is not part of trajectory
        assert(fabs(m1->x[j][0] - m2->x[i][0]) < THRESHOLD);
        assert(fabs(m1->x[j][1] - m2->x[i][1]) < THRESHOLD);
        assert(fabs(m1->x[j][2] - m2->x[i][2]) < THRESHOLD);
        assert(fabs(m1->v[j][0] + m2->v[i][0]) < THRESHOLD);
        assert(fabs(m1->v[j][1] + m2->v[i][1]) < THRESHOLD);
        assert(fabs(m1->v[j][2] + m2->v[i][2]) < THRESHOLD);
        assert(fabs(m1->v[j][3] - m2->v[i][3]) < THRESHOLD);
    }

    delete_missiles(trj);
    delete_planets(p);

    return 0;
}
