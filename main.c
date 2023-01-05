#include <pthread.h>
#include <sys/mman.h>

#include "Moduleur.h"
#include "Vehicule.h"

#define NB_VOITURE 200

struct Moduleur_sync_args args;

pthread_t tid_stats;
pthread_t tid_modulation;
pthread_t tid_creer_voiture[NB_VOITURE];

int main() {

    srand(time(NULL));

    // Initialise les variables conditions du moduleur, son mutex, etc
    Init_sync_args(&args);

    // Initialise un guichet par classe et initialise les files d'attente
    Init_guichets();

    // Crée le thread du moduleur
    Creer_thread_moduleur(&tid_modulation, &args);

    // Initialise le thread de statistiques, qui envoie des infos sur les guichets du péage
    Init_stats(&tid_stats, &args);

    // Création des véhicules
    int i, sleep_time = 150, compteur_rush = 6, est_heure_pleine = 0;
    sleep(2);
    for(i = 1; i < NB_VOITURE; ++i) {
        int *num = malloc(sizeof(*num));
        *num = i;
        Creer_thread_vehicule(*num, &args);
        usleep(sleep_time * 10000);
        --compteur_rush;
        if(compteur_rush == 0)
        {
            if(est_heure_pleine == 1) {
                sleep_time = 150;
                compteur_rush = 6;
                est_heure_pleine = 0;
            } else {
                sleep_time = 15;
                compteur_rush = 30;
                est_heure_pleine = 1;
            }
        }
    }

    // Join tous les threads des voitures
    for(i = 0; i < NB_VOITURE; ++i)
        pthread_join(tid_creer_voiture[i], NULL);

    // Join le thread modulation
    pthread_join(tid_modulation, NULL);

    // Join le thread stats
    pthread_join(tid_stats, NULL);

    return 0;
}
