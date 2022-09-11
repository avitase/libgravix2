#include "config.h"

#include <stdlib.h>

#include "api.h"

struct GrvxConfig *grvx_get_config(void)
{
    struct GrvxConfig *cfg = malloc(sizeof(struct GrvxConfig));

#if GRVX_POT_TYPE == GRVX_POT_TYPE_2D
    cfg->pot_type = "2D";
    cfg->n_pot = -1;
#elif GRVX_POT_TYPE == GRVX_POT_TYPE_3D
    cfg->pot_type = "3D";
    cfg->n_pot = GRVX_N_POT;
#endif

    cfg->trajectory_size = GRVX_TRAJECTORY_SIZE;
    cfg->int_steps = GRVX_INT_STEPS;
    cfg->min_dist = GRVX_MIN_DIST;
    cfg->composition_scheme = GRVX_COMPOSITION_SCHEME;
    cfg->n_stages = GRVX_COMPOSITION_STAGES;

    return cfg;
}

void grvx_free_config(struct GrvxConfig *cfg)
{
    free(cfg);
}
