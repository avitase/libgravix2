#include "api.h"
#include "integrators.h"
#include "planet.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

static const double THRESHOLD = MIN_DIST / 180. * M_PI;

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
    const double DEG2RAD = M_PI / 180.;
    lat *= DEG2RAD;
    lon *= DEG2RAD;

    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    t->x[0][0] = cos_lat * sin_lon;
    t->x[0][1] = cos_lat * cos_lon;
    t->x[0][2] = sin_lat;

    const double dv = sqrt(dlat * dlat + dlon * dlon);
    t->v[0][0] = (-dlat * sin_lat * sin_lon + dlon * cos_lon) / dv;
    t->v[0][1] = (-dlat * sin_lat * cos_lon - dlon * sin_lon) / dv;
    t->v[0][2] = dlat * cos_lat / dv;
    t->v[0][3] = v;

    for (int i = 0; i < 3; i++) {
        t->x[TRAJECTORY_SIZE - 1][i] = t->x[0][i];
        t->v[TRAJECTORY_SIZE - 1][i] = t->v[0][i];
    }
    t->v[TRAJECTORY_SIZE - 1][3] = v;

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
                   const PlanetsHandle planets,
                   unsigned planet,
                   double v,
                   double psi) {
    if (planet >= planets->n) {
        return -1;
    }

    const double DEG2RAD = M_PI / 180.;
    psi *= DEG2RAD;

    struct Vec3D rot[3];
    rotation_matrix(
        asin(planets->data[3 * planet + 2]),                             // lat.
        atan2(planets->data[3 * planet], planets->data[3 * planet + 1]), // lon.
        rot);

    const double sin_r = sin(THRESHOLD);
    const double cos_r = cos(THRESHOLD);
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

    const double lat = asin(x1.z);
    const double lon = atan2(x1.x, x1.y);

    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

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

    const double RAD2DEG = 180. / M_PI;
    const double dlat = dot(v1, e_lat);
    const double dlon = dot(v1, e_lon);
    return init_missile(t, lat * RAD2DEG, lon * RAD2DEG, v, dlat, dlon);
}

unsigned propagate_missile(struct Trajectory *trj,
                           const PlanetsHandle planets,
                           double h,
                           int *premature) {
    const double DEG2RAD = M_PI / 180.;
    const double RAD2DEG = 180. / M_PI;

    struct QP qp = {
        .q.x = trj->x[TRAJECTORY_SIZE - 1][0],
        .q.y = trj->x[TRAJECTORY_SIZE - 1][1],
        .q.z = trj->x[TRAJECTORY_SIZE - 1][2],
        .p.x = trj->v[TRAJECTORY_SIZE - 1][0],
        .p.y = trj->v[TRAJECTORY_SIZE - 1][1],
        .p.z = trj->v[TRAJECTORY_SIZE - 1][2],
        .p_abs = trj->v[TRAJECTORY_SIZE - 1][3] * DEG2RAD,
    };

    unsigned i = 0;
    for (*premature = 0; i < TRAJECTORY_SIZE && !*premature; i++) {
        unsigned n_left = integration_loop(&qp, h, INT_STEPS, planets);
        *premature = (n_left != 0);

        trj->x[i][0] = qp.q.x;
        trj->x[i][1] = qp.q.y;
        trj->x[i][2] = qp.q.z;
        trj->v[i][0] = qp.p.x;
        trj->v[i][1] = qp.p.y;
        trj->v[i][2] = qp.p.z;
        trj->v[i][3] = qp.p_abs * RAD2DEG;
    }

    return i;
}

double orb_period(double v, double h) {
    const double DEG2RAD = M_PI / 180.;
    v *= DEG2RAD;

    const double sin_threshold = sin(THRESHOLD);
    const double cos_threshold = cos(THRESHOLD);

    struct QP qp = {
        .q.x = 0.,
        .q.y = cos_threshold,
        .q.z = sin_threshold,
        .p.x = 0.,
        .p.y = -sin_threshold,
        .p.z = cos_threshold,
        .p_abs = v,
    };

    PlanetsHandle planets = new_planets(1);  // TODO(#7)
    set_planet(planets, 0, 0., 0.);

    double mdist = -1.;

    struct QP qp2 = qp;
    int t = 0;
    do {
        qp = qp2;
        mdist = integration_step(&qp2, h, planets);
        t += 1;
    } while (mdist < cos_threshold);

    delete_planets(planets);  // TODO(#7)

    const double s = acos(qp.q.y) - THRESHOLD;
    const double a = qp2.p_abs - qp.p_abs;
    const double dt = sqrt(2 * s / a);
    assert(dt < 1.);

    return ((double)t + dt) / INT_STEPS;
}
