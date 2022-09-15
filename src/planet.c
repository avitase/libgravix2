#include "planet.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include "api.h"

GrvxPlanetsHandle grvx_new_planets(unsigned n)
{
    struct GrvxPlanets *ptr = malloc(sizeof(struct GrvxPlanets));
    ptr->data = malloc(sizeof(double) * 3 * n);
    ptr->n = n;
    return ptr;
}

void grvx_delete_planets(GrvxPlanetsHandle p)
{
    free(p->data);
    free(p);
}

unsigned grvx_count_planets(GrvxPlanetsHandle p)
{
    return p->n;
}

int grvx_set_planet(GrvxPlanetsHandle p, unsigned i, double lat, double lon)
{
    if (i >= p->n) {
        return -1;
    }

    const double sin_lat = sin(lat);
    const double cos_lat = cos(lat);
    const double sin_lon = sin(lon);
    const double cos_lon = cos(lon);

    ptrdiff_t offset = 3 * (ptrdiff_t)i;
    p->data[offset] = cos_lat * sin_lon;
    p->data[offset + 1] = cos_lat * cos_lon;
    p->data[offset + 2] = sin_lat;

    return 0;
}

int grvx_get_planet(GrvxPlanetsHandle p, unsigned i, double *lat, double *lon)
{
    if (i >= p->n) {
        return -1;
    }

    ptrdiff_t offset = 3 * (ptrdiff_t)i;
    const double x = p->data[offset];
    const double y = p->data[offset + 1];
    const double z = p->data[offset + 2];

    *lat = asin(z);
    *lon = atan2(x, y);

    return 0;
}

unsigned grvx_pop_planet(GrvxPlanetsHandle p)
{
    if (p->n > 0) {
        p->n = p->n - 1;
    }
    return p->n;
}
