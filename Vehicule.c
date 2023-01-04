//
// Created by thibaud on 23/12/22.
//

#include <unistd.h>
#include "Vehicule.h"

Vehicule Creer_vehicule(int numero, int classe, int est_covoiturage, double distance_parcourue, int mode_paiement) {

    Vehicule v;
    v.numero = numero;
    v.classe = classe;
    v.est_voiturage = est_covoiturage;
    v.distance_parcourue = distance_parcourue;
    v.mode_paiement = mode_paiement;
    v.num_guichet = -1;

    return v;
}

void Creer_thread_vehicule(int num, struct Moduleur_sync_args * args) {

    pthread_t pid;
    struct infos * infos = malloc(sizeof(* infos));
    infos->numero = num;
    infos->args = args;
    pthread_create(&pid, NULL, thr_vehicule, (void *)infos);
}

void * thr_vehicule(void * p_args) {

    struct infos * infos = (struct infos *)p_args;
    int num = infos->numero;
    struct Moduleur_sync_args * args = infos->args;
    free(infos);

    double sleep_time, variance;

    Vehicule *v = malloc(sizeof(*v));
    *v = Creer_vehicule(num, rand() % 5, (rand() % 10 >= 2 ? 0 : 1), 50 + rand() % 450, rand() % 3);

    if(pthread_mutex_lock(&args->mutex) != 0)
        throw_error(__FILE__, __LINE__);
    args->compteur_voitures++;
    args->total_vehicules++;
    //printf(ANSI_COLOR_RED "La voiture %d de classe %d arrive au péage" ANSI_COLOR_RESET "\n", v->numero, v->classe);
    //printf( ANSI_COLOR_YELLOW "File d'attente du péage : %d" ANSI_COLOR_RESET "\n", args->compteur_voitures);
    if(pthread_cond_signal(&args->cond1) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_wait(&args->cond2, &args->mutex) != 0)
        throw_error(__FILE__, __LINE__);
    args->vehicule_courant = v;
    if(pthread_cond_signal(&args->cond3) != 0)
        throw_error(__FILE__, __LINE__);
    args->compteur_voitures--;
    //printf( ANSI_COLOR_YELLOW "File d'attente du péage : %d" ANSI_COLOR_RESET "\n", args->compteur_voitures);
    if(pthread_cond_wait(&args->cond4, &args->mutex) != 0)
        throw_error(__FILE__, __LINE__); // Attend son guichet

    if(pthread_mutex_lock(&v->guichet->mutex) != 0)
        throw_error(__FILE__, __LINE__);
    (*(v->guichet->file_d_attente))++;
    //printf( ANSI_COLOR_YELLOW "File d'attente du guichet %d %d : %d" ANSI_COLOR_RESET "\n", v->guichet->classe, v->guichet->numero, *v->guichet->file_d_attente);
    if(pthread_mutex_unlock(&args->mutex) != 0)
        throw_error(__FILE__, __LINE__);

    if(pthread_cond_signal(&v->guichet->cond1) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_wait(&v->guichet->cond2, &v->guichet->mutex) != 0)
        throw_error(__FILE__, __LINE__);
    //printf("Le vehicule %d %d passe au guichet %d %d\n", v->numero, v->classe, v->guichet->classe, v->guichet->numero);

    if(v->guichet->classe == 5 /*|| ((v->mode_paiement == 1 || v->mode_paiement == 2) && (v->guichet->mode_paiement == 1 || v->guichet->mode_paiement == 2))*/) {
        sleep_time = 0.2;
    } else {
        variance = ((double)(rand() % 400) - 200) / 1000;
        sleep_time = temps_attente[v->classe] + variance;
    }

    if(pthread_mutex_unlock(&v->guichet->mutex) != 0)
        throw_error(__FILE__, __LINE__);

    usleep((int)(sleep_time) * 1000000);

    if(pthread_mutex_lock(&v->guichet->mutex) != 0)
        throw_error(__FILE__, __LINE__);
    //printf("Le vehicule %d quitte le péage\n", v->numero);
    (*(v->guichet->file_d_attente))--;
    //printf( ANSI_COLOR_YELLOW "File d'attente du guichet %d %d : %d" ANSI_COLOR_RESET "\n", v->guichet->classe, v->guichet->numero, *v->guichet->file_d_attente);
    if(pthread_mutex_lock(&args->mutex) != 0)
        throw_error(__FILE__, __LINE__);
    args->total_vehicules--;
    if(pthread_mutex_unlock(&args->mutex) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_signal(&v->guichet->cond3) != 0)
        throw_error(__FILE__, __LINE__); // Signal qu'il est passé
    if(pthread_mutex_unlock(&v->guichet->mutex) != 0)
        throw_error(__FILE__, __LINE__);
    free(v);
    pthread_exit(0);
}
