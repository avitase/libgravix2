#include "config.h"
#include "api.h"
#include <stdlib.h>

struct Config *get_config(void) {
    struct Config *cfg = malloc(sizeof(struct Config));

#if POT_TYPE == POT_TYPE_2D
    cfg->pot_type = "2D";
    cfg->n_pot = -1;
#elif POT_TYPE == POT_TYPE_3D
    cfg->pot_type = "3D";
    cfg->n_pot = N_POT;
#endif

    cfg->trajectory_size = TRAJECTORY_SIZE;
    cfg->int_steps = INT_STEPS;
    cfg->min_dist = MIN_DIST;
    cfg->composition_scheme = COMPOSITION_SCHEME_STR;

    return cfg;
}

void free_config(struct Config *cfg) {
    free(cfg);
}
