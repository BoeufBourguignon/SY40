#include <pthread.h>
#include <sys/mman.h>

#include "Moduleur.h"
#include "Vehicule.h"

#define NB_VOITURE 50

struct Moduleur_sync_args args;

pthread_t tid_stats;
pthread_t tid_modulation;
pthread_t tid_creer_voiture[NB_VOITURE];

/* TODO DONE moyens de paiements véhicule & bornes */
/* TODO DONE temps de paiement différent selon : la classe, le mode de paiement */
/* TODO DONE guichet covoiturage */
/* TODO création véhicule ne s'arrete pas et aléatoire */
/* TODO heures pleines : toutes les x secondes, on envoie vla des véhicules ; heures creuses : véhicules arrivent de manière constante */
/* TODO SIGINT nettoie correctement la mémoire */

/* TODO réviser tout le code, supprimer les var globales & fonctions inutiles */

int main() {

    srand(time(NULL));

    Init_sync_args(&args);

    Init_guichets();

    Creer_thread_moduleur(&tid_modulation, &args);

    Init_stats(&tid_stats, &args);

    int i, sleep_time = 10;
    // Création des véhicules
    sleep(2);
    for(i = 1; i < NB_VOITURE; ++i) {
        int *num = malloc(sizeof(*num));
        *num = i;
        Creer_thread_vehicule(*num, &args);
        usleep(sleep_time * 100000);
        if(i % 10 == 0)
        {
            sleep_time = 2;
        }
        if(i % 20 == 0)
        {
            sleep_time = 10;
        }
    }

    // Join tous les threads des voitures
    for(i = 0; i < NB_VOITURE; ++i)
        pthread_join(tid_creer_voiture[i], NULL);

    // Join le thread modulation
    // (il finit jamais donc le join sera jamais résolu, c'est juste pour pas que le programme se termine)
    pthread_join(tid_modulation, NULL);

    return 0;
}
