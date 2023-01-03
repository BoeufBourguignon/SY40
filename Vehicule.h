//
// Created by thibaud on 23/12/22.
//

#ifndef PROJETA22_VEHICULE_H
#define PROJETA22_VEHICULE_H

#include "Colors.h"
#include "Peage_utils.h"

#include <malloc.h>
#include <string.h>
#include <stdlib.h>

static double temps_attente[] = {1, 2, 3, 4, 0.5};

struct infos {
    int numero;
    struct Moduleur_sync_args * args;
};

Vehicule
Creer_vehicule(
        int numero,
        int classe,
        int est_covoiturage,
        double distance_parcourue,
        int mode_paiement
);

void
Creer_thread_vehicule(
        int num,
        struct Moduleur_sync_args * args
);

static
void *
thr_vehicule(
        void * p_args
);

#endif //PROJETA22_VEHICULE_H
