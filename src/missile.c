#include "api.h"
#include "integrators.h"
#include "planet.h"
#include "pot.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

TrajectoryBatch new_missiles(unsigned n) {
    return malloc(sizeof(struct Trajectory) * n);
}

void delete_missiles(TrajectoryBatch batch) {
    free(batch);
}

struct Trajectory *get_trajectory(TrajectoryBatch batch, unsigned i) {
    return batch + i;
}

int init_missile(struct Trajectory *t,
                 double lat,
                 double lon,
                 double v,
                 double dlat,
                 double dlon) {
    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    t->x[0][0] = cos_lat * sin_lon;
    t->x[0][1] = cos_lat * cos_lon;
    t->x[0][2] = sin_lat;

    const double dv = sqrt(dlat * dlat + dlon * dlon);
    t->v[0][0] = v * (-dlat * sin_lat * sin_lon + dlon * cos_lon) / dv;
    t->v[0][1] = v * (-dlat * sin_lat * cos_lon - dlon * sin_lon) / dv;
    t->v[0][2] = v * dlat * cos_lat / dv;

    for (int i = 0; i < 3; i++) {
        t->x[TRAJECTORY_SIZE - 1][i] = t->x[0][i];
        t->v[TRAJECTORY_SIZE - 1][i] = t->v[0][i];
    }

    return 0;
}

static void rotation_matrix(double lat, double lon, struct Vec3D rot[3]) {
    double sin_lat = sin(lat);
    double cos_lat = cos(lat);
    double sin_lon = sin(lon);
    double cos_lon = cos(lon);

    struct Vec3D r1 = {
        -cos_lon,
        -sin_lat * sin_lon,
        cos_lat * sin_lon,
    };
    struct Vec3D r2 = {
        sin_lon,
        -sin_lat * cos_lon,
        cos_lat * cos_lon,
    };
    struct Vec3D r3 = {
        0.,
        cos_lat,
        sin_lat,
    };

    rot[0] = r1;
    rot[1] = r2;
    rot[2] = r3;
}

int launch_missile(struct Trajectory *t,
                   PlanetsHandle planets,
                   unsigned planet,
                   double v,
                   double psi) {
    if (planet >= planets->n) {
        return -1;
    }

    struct Vec3D rot[3];
    rotation_matrix(
        lat(planets->data[3 * planet + 2]),
        lon(planets->data[3 * planet], planets->data[3 * planet + 1]), rot);

    const double sin_r = sin(MIN_DIST);
    const double cos_r = cos(MIN_DIST);
    const double sin_psi = sin(psi);
    const double cos_psi = cos(psi);

    struct Vec3D x0 = {
        sin_r * sin_psi,
        sin_r * cos_psi,
        cos_r,
    };
    struct Vec3D x1 = {
        dot(rot[0], x0),
        dot(rot[1], x0),
        dot(rot[2], x0),
    };
    struct Vec3D v0 = {
        cos_r * sin_psi,
        cos_r * cos_psi,
        -sin_r,
    };
    struct Vec3D v1 = {
        dot(rot[0], v0),
        dot(rot[1], v0),
        dot(rot[2], v0),
    };

    const double lat1 = lat(x1.z);
    const double lon1 = lon(x1.x, x1.y);

    const double sin_lat = sin(lat1);
    const double cos_lat = cos(lat1);
    const double sin_lon = sin(lon1);
    const double cos_lon = cos(lon1);

    struct Vec3D e_lat = {
        -sin_lat * sin_lon,
        -sin_lat * cos_lon,
        cos_lat,
    };
    struct Vec3D e_lon = {
        cos_lon,
        -sin_lon,
        0.,
    };

    const double dlat1 = dot(v1, e_lat);
    const double dlon1 = dot(v1, e_lon);
    return init_missile(t, lat1, lon1, v, dlat1, dlon1);
}

unsigned propagate_missile(struct Trajectory *trj,
                           PlanetsHandle planets,
                           double h,
                           int *premature) {
    struct QP qp = {
        .q.x = trj->x[TRAJECTORY_SIZE - 1][0],
        .q.y = trj->x[TRAJECTORY_SIZE - 1][1],
        .q.z = trj->x[TRAJECTORY_SIZE - 1][2],
        .p.x = trj->v[TRAJECTORY_SIZE - 1][0],
        .p.y = trj->v[TRAJECTORY_SIZE - 1][1],
        .p.z = trj->v[TRAJECTORY_SIZE - 1][2],
    };

    unsigned i = 0;
    for (*premature = 0; i < TRAJECTORY_SIZE && !*premature; i++) {
        unsigned n_left = integration_loop(&qp, h, INT_STEPS, planets);
        *premature = (n_left != 0);

        assert(fabs(dot(qp.q, qp.q) - 1.) < 1e-10);
        assert(fabs(dot(qp.p, qp.q)) < 1e-10);

        trj->x[i][0] = qp.q.x;
        trj->x[i][1] = qp.q.y;
        trj->x[i][2] = qp.q.z;
        trj->v[i][0] = qp.p.x;
        trj->v[i][1] = qp.p.y;
        trj->v[i][2] = qp.p.z;
    }

    return i;
}

double orb_period(double v, double h) {
    const double sin_threshold = sin(MIN_DIST);
    const double cos_threshold = cos(MIN_DIST);

    struct QP qp = {
        .q.x = 0.,
        .q.y = cos_threshold,
        .q.z = sin_threshold,
        .p.x = 0.,
        .p.y = -v * sin_threshold,
        .p.z = v * cos_threshold,
    };

    struct Planets p;
    double planets_data[3];
    p.data = planets_data;
    p.n = 1;

    PlanetsHandle planets = &p;
    set_planet(planets, 0, 0., 0.);

    double mdist = -1.;

    struct QP qp2 = qp;
    struct QP e = {
        0., 0., 0., 0., 0., 0.,
    };
    int t = 0;
    do {
        qp = qp2;
        integration_step(&qp2, &e, h, planets);
        mdist = min_dist(&qp2.q, planets);
        t += 1;
    } while (mdist < cos_threshold);

    const double s = acos(qp.q.y) - MIN_DIST;
    assert(s > 0.);

    const double a = mag(qp2.p) - mag(qp.p);
    assert(a > 0.);

    const double dt = sqrt(2 * s / a);
    assert(!isnan(dt) && dt < 1.);

    return ((double)t + dt) / INT_STEPS;
}
