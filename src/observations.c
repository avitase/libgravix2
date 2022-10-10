#include "libgravix2/observations.h"
#include <stdlib.h>

struct GrvxMissileObservations *
add_observation(struct GrvxMissileObservations *head,
                struct GrvxMissileObservation *obs)
{
    struct GrvxMissileObservations *new_node =
        malloc(sizeof(struct GrvxMissileObservations));
    new_node->obs = obs;
    new_node->next = 0;

    if (head == 0) {
        return new_node;
    }

    if (head->obs->t > obs->t) {
        new_node->next = head;
        return new_node;
    }

    struct GrvxMissileObservations *ptr = head;
    while (ptr->next != 0) {
        if (ptr->next->obs->t > obs->t) {
            new_node->next = ptr->next;
            ptr->next = new_node;
            return head;
        }

        ptr = ptr->next;
    }

    ptr->next = new_node;
    return head;
}

struct GrvxMissileObservations *
pop_observation(struct GrvxMissileObservations *head,
                struct GrvxMissileObservation **observation)
{
    if (head == 0) {
        observation = 0;
        return head;
    }

    *observation = head->obs;

    struct GrvxMissileObservations *old = head;
    head = old->next;
    free(old);

    return head;
}

void delete_observations(struct GrvxMissileObservations *head)
{
    while (head != 0) {
        free(head->obs);

        struct GrvxMissileObservations *next = head->next;
        free(head);

        head = next;
    }
}
