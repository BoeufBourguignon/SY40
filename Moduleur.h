/**
 * @author Thibaud Leclere
 * @date 22/12/22
 */

#ifndef PROJETA22_MODULEUR_H
#define PROJETA22_MODULEUR_H

#include <unistd.h>

#include "Colors.h"
#include "Peage_utils.h"
#include "Guichet.h"

#define SEUIL_ALERTE 3 // Quand il y a ce nombre de bagnoles à un guichet, on en ouvre un autre (sauf pour covoiturage)
#define NB_GUICHET_PAR_CLASSE 2 // Nombre maximal de guichet ouvrable par classe de véhicule
#define SEUIL_GUICHET_COVOIT 5

static pthread_t pid_guichets[5][NB_GUICHET_PAR_CLASSE];
static int etat_guichets[5][NB_GUICHET_PAR_CLASSE]; // 1 si guichet ouvert, sinon 0
static int files_d_attente[5][NB_GUICHET_PAR_CLASSE]; // Compte les véhicules à chaque guichet
static Guichet * struct_guichets[5][NB_GUICHET_PAR_CLASSE];
static double distance_guichets[5][NB_GUICHET_PAR_CLASSE];
static int total_guichets[5][NB_GUICHET_PAR_CLASSE];

static pthread_t pid_guichet_coivoit;
static int etat_guichet_covoit;
static int file_guichet_covoit;
static Guichet * guichet_covoit;
static double distance_guichet_covoit;
static int total_guichet_covoit;

void
Init_guichets(
);

/**
 * Initialise les arguments de synchronisation entre le moduleur et les véhicules
 * @param args
 */
void
Init_sync_args(
        struct Moduleur_sync_args * args
);

/**
 * Initialise un thread envoyant des statistiques sur les différents guichets
 * @param pid_stats
 * @param args
 */
void
Init_stats(
        pthread_t * pid_stats,
        struct Moduleur_sync_args * args
);

/**
 * Créer le thread du moduleur, qui récupère les véhicules qui arrivent au péage pour les envoyer vers le bon guichet
 * @param tid
 * @param args
 */
void
Creer_thread_moduleur(
        pthread_t * tid,
        struct Moduleur_sync_args * args
);

/**
 * Thread de modulation, qui récupère les véhicules qui arrivent au péage pour les envoyer vers le bon guichet
 * S'il n'y a pas de véhicule dans la file d'attente, le thread attend d'être réveillé
 * Sinon, le thread signal au véhicule l'attendant qu'il l'écoute
 * A ce signal, le vehicule envoie au moduleur ses informations et le signal au moduleur
 * A ce signal, le moduleur récupère les infos du vehicule et lui attribue un guichet. Il le signal ensuite au véhicule
 * @param p_args
 * @return
 */
_Noreturn
static
void *
thr_modulation(
        void * p_args
);

/**
 * Thread de statistiques, qui print des informations sur tous les guichets
 * @param p_args
 * @return
 */
_Noreturn
static
void *
thr_stats(
        void * p_args
);

/**
 * Méthode utilisée par le moduleur pour orienter les véhicules se présentants au péage
 * Vérifie le total de véhicules en file d'attente du péage, pour ouvrir la voie de covoiturage
 * Si le véhicule ne peut pas emprunter la voie de covoiturage :
 *   Est redirigé vers le guichet de la classe du véhicule
 *   Si le guichet est surchargé, on regarde s'il y d'autres guichets ouverts n'étant pas saturés
 *   Sinon, si on ouvre un nouveau guichet si on peut
 *   Sinon, le véhicule va vers la file la moins pire
 * @param args
 */
static
void
Modulation(
        struct Moduleur_sync_args * args
);

/**
 * Ouvre un guichet correspondant aux paramètres
 * @param classe
 * @param num_guichet
 * @param mode_paiement
 */
static
void
Moduleur_Ouvrir_Guichet(
        int classe,
        int num_guichet,
        int mode_paiement
);

/**
 * Ouvre le guichet de covoiturage
 */
static
void
Moduleur_Ouvrir_Guichet_Covoiturage(
);

/**
 * Ferme un certain guichet, selon sa classe et son numéro
 * @param classe
 * @param num_guichet
 */
static
void
Moduleur_Fermer_Guichet(
        int classe,
        int num_guichet
);

/**
 * Ferme le guichet de covoiturage
 */
static
void
Moduleur_Fermer_Guichet_Covoiturage(
);

/**
 * Récupère et affiche des statistiques sur les guichets
 */
static
void
getStats(
);

#endif //PROJETA22_MODULEUR_H
