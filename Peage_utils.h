//
// Created by thibaud on 23/12/22.
//

#ifndef PROJETA22_PEAGE_UTILS_H
#define PROJETA22_PEAGE_UTILS_H

#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    pthread_t * pid;
    int numero;
    int classe;
    int mode_paiement;
    pthread_mutex_t mutex;
    pthread_cond_t cond1;
    pthread_cond_t cond2;
    pthread_cond_t cond3;
    int * file_d_attente;
} Guichet;

typedef struct {
    int numero;
    int classe;
    int est_voiturage;
    double distance_parcourue;
    int num_guichet;
    int mode_paiement;
    Guichet * guichet;
} Vehicule;

struct Moduleur_sync_args {
    pthread_cond_t cond1;
    pthread_cond_t cond2;
    pthread_cond_t cond3;
    pthread_cond_t cond4;
    pthread_cond_t cond5;
    pthread_mutex_t mutex_voiture;
    int compteur_voitures;
    Vehicule *vehicule_courant;
    int total_vehicules;
    int compteur_sortie;
};

void throw_error(char * file, int line);

#endif //PROJETA22_PEAGE_UTILS_H
