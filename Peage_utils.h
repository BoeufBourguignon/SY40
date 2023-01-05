/**
 * @author Thibaud Leclere
 * @date 23/12/22
 */

#ifndef PROJETA22_PEAGE_UTILS_H
#define PROJETA22_PEAGE_UTILS_H

#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Structure d'un seul guichet
 * Elle est communiquée à un véhicule par le moduleur
 */
typedef struct {
    pthread_t * pid;        // Pid du thread du guichet
    int numero;             // Numéro du guichet, relatif à la classe du guichet
    int classe;             // Classe du guichet. Correspond à la classe de véhicule que le guichet accepte
    int mode_paiement;      // Mode de paiement accepté par le guichet (0 = espèces ou CB, 1 = abonnement, 3 = les deux)
    pthread_mutex_t mutex;  // Mutex de ce guichet
    pthread_cond_t cond1;   // Première condition de synchronisation, permettant au véhicule de le réveiller lorsque celui ci arrive
    pthread_cond_t cond2;   // Deuxième condition de synchronisation, permettant au guichet d'accueillir le vehicule
    pthread_cond_t cond3;   // Troisième condition de synchronisation, permettant au véhicule de signaler qu'il est passé et donc que le guichet est à nouveau libre
    int * file_d_attente;   // File d'attente du guichet, utilisée pour savoir si le guichet peut "s'endormir" s'il n'y a personne
} Guichet;

/**
 * Structure représentant un véhicule
 */
typedef struct {
    int numero;
    int classe;
    int est_voiturage;
    double distance_parcourue;
    int mode_paiement;
    // Après être passé par le moduleur :
    int num_guichet;
    Guichet * guichet;          // Pointeur vers la structure du guichet attribué par le moduleur.
                                // Utilisé pour lui augmenter la file d'attente et se synchroniser avec le conditions et le mutex
} Vehicule;

/**
 * Structure contenant les éléments de synchronisation entre les véhicules arrivant au péage et le moduleur
 * Le moduleur redirige les véhicules vers les bons guichets, et gère les guichets de façon optimale
 */
struct Moduleur_sync_args {
    pthread_cond_t cond1;           // Première condition de synchronisation, permettant au véhicule de le réveiller lorsque celui ci arrive
    pthread_cond_t cond2;           // Deuxième condition de synchronisation, permettant de demander au véhicule de partager sa structure
    pthread_cond_t cond3;           // Troisième condition de synchronisation, permettant au véhicule d'indiquer qu'il a partagé sa structure
    pthread_cond_t cond4;           // Quatrième condition de synchronisation, permettant au moduleur d'indiquer qu'un guichet a été attribué au véhicule
    pthread_mutex_t mutex;          // Mutex du moduleur
    int compteur_voitures;          // Nombre de voitures attendant d'être modulées
    Vehicule *vehicule_courant;     // Pointeur vers le véhicule en cours de traitement par le moduleur
    int total_vehicules;            // Total de véhicules présents au péage (guichets compris)
};

/**
 * Envoi une erreur, affichant également le fichier et la ligne où a eu lieu l'erreur
 * @param file
 * @param line
 */
void throw_error(char * file, int line);

#endif //PROJETA22_PEAGE_UTILS_H
