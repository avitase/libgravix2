#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include "libgravix2/api.h"
#include "libgravix2/config.h"
#include "libgravix2/integrators.h"
#include "libgravix2/planet.h"
#include "libgravix2/pot.h"

GrvxTrajectoryBatch grvx_new_missiles(unsigned n)
{
    return malloc(sizeof(struct GrvxTrajectory) * n);
}

void grvx_delete_missiles(GrvxTrajectoryBatch batch)
{
    free(batch);
}

struct GrvxTrajectory *grvx_get_trajectory(GrvxTrajectoryBatch batch,
                                           unsigned i)
{
    return batch + i;
}

int grvx_init_missile(struct GrvxTrajectory *t,
                      double lat,
                      double lon,
                      double v,
                      double dlat,
                      double dlon)
{
    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    t->x[0][0] = cos_lat * sin_lon;
    t->x[0][1] = cos_lat * cos_lon;
    t->x[0][2] = sin_lat;

    const double dv = sqrt(dlat * dlat + dlon * dlon);
    assert(dv > 0.);
    t->v[0][0] = v * (-dlat * sin_lat * sin_lon + dlon * cos_lon) / dv;
    t->v[0][1] = v * (-dlat * sin_lat * cos_lon - dlon * sin_lon) / dv;
    t->v[0][2] = v * dlat * cos_lat / dv;

    for (int i = 0; i < 3; i++) {
        t->x[GRVX_TRAJECTORY_SIZE - 1][i] = t->x[0][i];
        t->v[GRVX_TRAJECTORY_SIZE - 1][i] = t->v[0][i];
    }

    return 0;
}

static void rotation_matrix(double lat, double lon, struct GrvxVec3D rot[3])
{
    double sin_lat = sin(lat);
    double cos_lat = cos(lat);
    double sin_lon = sin(lon);
    double cos_lon = cos(lon);

    struct GrvxVec3D r1 = {
        -cos_lon,
        -sin_lat * sin_lon,
        cos_lat * sin_lon,
    };
    struct GrvxVec3D r2 = {
        sin_lon,
        -sin_lat * cos_lon,
        cos_lat * cos_lon,
    };
    struct GrvxVec3D r3 = {
        0.,
        cos_lat,
        sin_lat,
    };

    rot[0] = r1;
    rot[1] = r2;
    rot[2] = r3;
}

static void prepare_launch(GrvxPlanetsHandle planets,
                           unsigned planet,
                           double psi,
                           struct GrvxVec3D *x,
                           struct GrvxVec3D *v)
{
    struct GrvxVec3D rot[3];
    ptrdiff_t offset = 3 * (ptrdiff_t)planet;
    rotation_matrix(grvx_lat(planets->data[offset + 2]),
                    grvx_lon(planets->data[offset], planets->data[offset + 1]),
                    rot);

    const double sin_r = sin(GRVX_MIN_DIST);
    const double cos_r = cos(GRVX_MIN_DIST);
    const double sin_psi = sin(psi);
    const double cos_psi = cos(psi);

    struct GrvxVec3D x0 = {
        sin_r * sin_psi,
        sin_r * cos_psi,
        cos_r,
    };

    struct GrvxVec3D v0 = {
        cos_r * sin_psi,
        cos_r * cos_psi,
        -sin_r,
    };

    x->x = grvx_dot(rot[0], x0);
    x->y = grvx_dot(rot[1], x0);
    x->z = grvx_dot(rot[2], x0);

    v->x = grvx_dot(rot[0], v0);
    v->y = grvx_dot(rot[1], v0);
    v->z = grvx_dot(rot[2], v0);
}

int grvx_launch_missile(struct GrvxTrajectory *t,
                        GrvxPlanetsHandle planets,
                        unsigned planet,
                        double v_abs,
                        double psi)
{
    if (planet >= planets->n) {
        return -1;
    }

    struct GrvxVec3D x;
    struct GrvxVec3D v;
    prepare_launch(planets, planet, psi, &x, &v);

    const double lat = grvx_lat(x.z);
    const double lon = grvx_lon(x.x, x.y);

    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    struct GrvxVec3D e_lat = {
        -sin_lat * sin_lon,
        -sin_lat * cos_lon,
        cos_lat,
    };
    struct GrvxVec3D e_lon = {
        cos_lon,
        -sin_lon,
        0.,
    };

    const double dlat = grvx_dot(v, e_lat);
    const double dlon = grvx_dot(v, e_lon);
    return grvx_init_missile(t, lat, lon, v_abs, dlat, dlon);
}

unsigned grvx_propagate_missile(struct GrvxTrajectory *trj,
                                GrvxPlanetsHandle planets,
                                double h,
                                int *premature)
{
    struct GrvxQP qp = {
        .q.x = trj->x[GRVX_TRAJECTORY_SIZE - 1][0],
        .q.y = trj->x[GRVX_TRAJECTORY_SIZE - 1][1],
        .q.z = trj->x[GRVX_TRAJECTORY_SIZE - 1][2],
        .p.x = trj->v[GRVX_TRAJECTORY_SIZE - 1][0],
        .p.y = trj->v[GRVX_TRAJECTORY_SIZE - 1][1],
        .p.z = trj->v[GRVX_TRAJECTORY_SIZE - 1][2],
    };

    unsigned i = 0;
    for (*premature = 0; i < GRVX_TRAJECTORY_SIZE && !*premature; i++) {
        unsigned n_left =
            grvx_integration_loop(&qp, h, GRVX_INT_STEPS, planets);
        *premature = (n_left != 0);

        assert(fabs(grvx_dot(qp.q, qp.q) - 1.) < 1e-10);
        assert(fabs(grvx_dot(qp.p, qp.q)) < 1e-10);

        trj->x[i][0] = qp.q.x;
        trj->x[i][1] = qp.q.y;
        trj->x[i][2] = qp.q.z;
        trj->v[i][0] = qp.p.x;
        trj->v[i][1] = qp.p.y;
        trj->v[i][2] = qp.p.z;
    }

    return i;
}

double grvx_orb_period(double v, double h)
{
    const double sin_threshold = sin(GRVX_MIN_DIST);
    const double cos_threshold = cos(GRVX_MIN_DIST);

    struct GrvxQP qp = {
        .q.x = 0.,
        .q.y = cos_threshold,
        .q.z = sin_threshold,
        .p.x = 0.,
        .p.y = -v * sin_threshold,
        .p.z = v * cos_threshold,
    };

    struct GrvxPlanets p;
    double planets_data[3];
    p.data = planets_data;
    p.n = 1;

    GrvxPlanetsHandle planets = &p;
    grvx_set_planet(planets, 0, 0., 0.);

    double mdist = -1.;

    struct GrvxQP qp2 = qp;
    struct GrvxQP e = {{0., 0., 0.}, {0., 0., 0.}};
    int t = 0;
    do {
        qp = qp2;
        grvx_integration_step(&qp2, &e, h, planets);
        mdist = grvx_min_dist(&qp2.q, planets);
        t += 1;
    } while (mdist < cos_threshold);

    const double s = acos(qp.q.y) - GRVX_MIN_DIST;
    assert(s > 0.);

    const double a = grvx_mag(qp2.p) - grvx_mag(qp.p);
    assert(a > 0.);

    const double dt = sqrt(2 * s / a);
    assert(!isnan(dt) && dt < 1.);

    return ((double)t + dt) / (double)GRVX_INT_STEPS;
}
