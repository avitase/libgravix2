#include "api.h"
#include "integrators.h"
#include "planet.h"
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

int init_missile(
    struct Trajectory *t, double lat, double lon, double vlat, double vlon) {
    const double DEG2RAD = M_PI / 180.;
    lat *= DEG2RAD;
    lon *= DEG2RAD;
    vlat *= DEG2RAD;
    vlon *= DEG2RAD; // WARNING: this is \f$\dot\lambda \, \cos\phi\f$

    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    const unsigned i = TRAJECTORY_SIZE - 1;
    t->x[i][0] = cos_lat * sin_lon;
    t->x[i][1] = cos_lat * cos_lon;
    t->x[i][2] = sin_lat;
    t->v[i][0] = -vlat * sin_lat * sin_lon + vlon * cos_lon;
    t->v[i][1] = -vlat * sin_lat * cos_lon - vlon * sin_lon;
    t->v[i][2] = vlat * cos_lat;

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
    v *= DEG2RAD;
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
        v * cos_r * sin_psi,
        v * cos_r * cos_psi,
        -v * sin_r,
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
    const double vlat = dot(v1, e_lat) * RAD2DEG;
    const double vlon = dot(v1, e_lon) * RAD2DEG;
    return init_missile(t, lat * RAD2DEG, lon * RAD2DEG, vlat, vlon);
}

unsigned propagate_missile(struct Trajectory *t,
                           const PlanetsHandle p,
                           double h,
                           int *premature) {
    for (unsigned i = 0; i < 3; i++) {
        t->x[0][i] = t->x[TRAJECTORY_SIZE - 1][i];
        t->v[0][i] = t->v[TRAJECTORY_SIZE - 1][i];
    }

    struct QP qp = {
        .q.x = t->x[0][0],
        .q.y = t->x[0][1],
        .q.z = t->x[0][2],
        .p.x = t->v[0][0],
        .p.y = t->v[0][1],
        .p.z = t->v[0][2],
    };

    unsigned i = 1;
    for (*premature = 0; i < TRAJECTORY_SIZE && !*premature; i++) {
        unsigned n_left = integration_loop(&qp, h, INT_STEPS, p);
        *premature = (n_left != 0);

        t->x[i][0] = qp.q.x;
        t->x[i][1] = qp.q.y;
        t->x[i][2] = qp.q.z;
        t->v[i][0] = qp.p.x;
        t->v[i][1] = qp.p.y;
        t->v[i][2] = qp.p.z;
    }

    return i;
}

double orb_period(double v0, double h) {
    const double sin_threshold = sin(THRESHOLD);
    const double cos_threshold = cos(THRESHOLD);

    struct QP qp = {
        .q.x = 0.,
        .q.y = cos_threshold,
        .q.z = sin_threshold,
        .p.x = 0.,
        .p.y = -v0 * sin_threshold,
        .p.z = v0 * cos_threshold,
    };

    PlanetsHandle planets = new_planets(1);
    set_planet(planets, 0, 0., 0.);

    double mdist = -1.;

    double p2;
    int t = 0;
    do {
        p2 = qp.p.y * qp.p.y + qp.p.z * qp.p.z;
        mdist = integration_step(&qp, h, planets);
        t += 1;
    } while (mdist < cos_threshold);

    delete_planets(planets);

    const double a = sqrt(qp.p.y * qp.p.y + qp.p.z * qp.p.z) - sqrt(p2);
    const double s = acos(qp.q.y) - THRESHOLD;
    return (double)t + sqrt(2 * s / a);
}
