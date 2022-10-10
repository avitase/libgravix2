#pragma once

#include "libgravix2/game.h"

struct GrvxMissileObservations {
    struct GrvxMissileObservation *obs;
    struct GrvxMissileObservations *next;
};

struct GrvxMissileObservations *
add_observation(struct GrvxMissileObservations *,
                struct GrvxMissileObservation *);

struct GrvxMissileObservations *
pop_observation(struct GrvxMissileObservations *,
                struct GrvxMissileObservation **);

void delete_observations(struct GrvxMissileObservations *);
