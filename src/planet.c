#include "planet.h"
#include "api.h"
#include <math.h>
#include <stdlib.h>

GrvxPlanetsHandle grvx_new_planets(unsigned n) {
    struct GrvxPlanets *ptr = malloc(sizeof(struct GrvxPlanets));
    ptr->data = malloc(sizeof(double) * 3 * n);
    ptr->n = n;
    return ptr;
}

void grvx_delete_planets(GrvxPlanetsHandle p) {
    free(p->data);
    free(p);
}

int grvx_set_planet(GrvxPlanetsHandle p, unsigned i, double lat, double lon) {
    if (i < p->n) {
        const double sin_lat = sin(lat);
        const double cos_lat = cos(lat);
        const double sin_lon = sin(lon);
        const double cos_lon = cos(lon);

        p->data[3 * i] = cos_lat * sin_lon;
        p->data[3 * i + 1] = cos_lat * cos_lon;
        p->data[3 * i + 2] = sin_lat;

        return 0;
    }

    return -1;
}

unsigned grvx_pop_planet(GrvxPlanetsHandle p) {
    if (p->n > 0) {
        p->n = p->n - 1;
    }
    return p->n;
}
