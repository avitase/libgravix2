#include "api.h"
#include "planet.h"
#include <assert.h>
#include <math.h>

int main(int argc, char **argv) {
    const double threshold = 1e-10;

    struct GrvxPlanets *p = grvx_new_planets(4);
    assert(p->n == 4);

    assert(grvx_set_planet(p, 0, 0., 1.) == 0);
    assert(grvx_set_planet(p, 1, 2., 3.) == 0);
    assert(grvx_set_planet(p, 2, 4., 5.) == 0);
    assert(grvx_set_planet(p, 3, 6., 7.) == 0);

    for (unsigned i = 0; i < p->n; i++) {
        const double lat = 2. * (double)i;
        const double lon = lat + 1.;

        const double sin_lat = sin(lat);
        const double cos_lat = cos(lat);
        const double sin_lon = sin(lon);
        const double cos_lon = cos(lon);

        assert(fabs(p->data[3 * i] - cos_lat * sin_lon) < threshold);
        assert(fabs(p->data[3 * i + 1] - cos_lat * cos_lon) < threshold);
        assert(fabs(p->data[3 * i + 2] - sin_lat) < threshold);
    }

    unsigned n = grvx_pop_planet(p);
    assert(n == p->n);
    assert(n == 3);

    for (unsigned i = 0; i < p->n; i++) {
        const double lat = 2. * (double)i;
        const double lon = lat + 1.;

        const double sin_lat = sin(lat);
        const double cos_lat = cos(lat);
        const double sin_lon = sin(lon);
        const double cos_lon = cos(lon);

        assert(fabs(p->data[3 * i] - cos_lat * sin_lon) < threshold);
        assert(fabs(p->data[3 * i + 1] - cos_lat * cos_lon) < threshold);
        assert(fabs(p->data[3 * i + 2] - sin_lat) < threshold);
    }

    assert(grvx_pop_planet(p) == 2);
    assert(p->n == 2);

    assert(grvx_pop_planet(p) == 1);
    assert(p->n == 1);

    const double lat = -1.;
    const double lon = -2.;
    assert(grvx_set_planet(p, 4, lat, lon) != 0);
    assert(grvx_set_planet(p, 0, lat, lon) == 0);
    assert(p->n == 1);

    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);
    assert(fabs(p->data[0] - cos_lat * sin_lon) < threshold);
    assert(fabs(p->data[1] - cos_lat * cos_lon) < threshold);
    assert(fabs(p->data[2] - sin_lat) < threshold);

    assert(grvx_pop_planet(p) == 0);
    assert(p->n == 0);

    grvx_delete_planets(p);

    return 0;
}
