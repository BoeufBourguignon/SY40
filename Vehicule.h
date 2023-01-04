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

/**
 * Temps d'attente moyen pour chaque classe (les classes sont les indices du tableau)
 */
static double temps_attente[] = {1, 2, 3, 4, 0.5};

/**
 * Structure permettant de communiquer au véhicule son numéro et la structure contenant les variables de synchronisation
 */
struct infos {
    int numero;
    struct Moduleur_sync_args * args;
};

/**
 * Crée une structure de vehicule correspondant aux paramètres
 * @param numero
 * @param classe
 * @param est_covoiturage
 * @param distance_parcourue
 * @param mode_paiement
 * @return
 */
Vehicule
Creer_vehicule(
        int numero,
        int classe,
        int est_covoiturage,
        double distance_parcourue,
        int mode_paiement
);

/**
 * Crée un thread pour un seul véhicule
 * @param num
 * @param args
 */
void
Creer_thread_vehicule(
        int num,
        struct Moduleur_sync_args * args
);

/**
 * Thread d'un véhicule
 * @param p_args
 * @return
 */
static
void *
thr_vehicule(
        void * p_args
);

#endif //PROJETA22_VEHICULE_H
