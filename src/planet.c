#include "planet.h"
#include "api.h"
#include <math.h>
#include <stdlib.h>

PlanetsHandle new_planets(unsigned n) {
    struct Planets *ptr = malloc(sizeof(struct Planets));
    ptr->data = malloc(sizeof(double) * 3 * n);
    ptr->n = n;
    return ptr;
}

void delete_planets(PlanetsHandle p) {
    free(p->data);
    free(p);
}

int set_planet(PlanetsHandle p, unsigned i, double lat, double lon) {
    if (i < p->n) {
        const double DEG2RAD = M_PI / 180.;
        lat *= DEG2RAD;
        lon *= DEG2RAD;

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

unsigned pop_planet(PlanetsHandle p) {
    if (p->n > 0) {
        p->n = p->n - 1;
    }
    return p->n;
}
