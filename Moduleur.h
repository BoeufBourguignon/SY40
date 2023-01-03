//
// Created by thibaud on 22/12/22.
//

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

static pthread_t pid_guichet_coivoit;
static int etat_guichet_covoit;
static int file_guichet_covoit;
static Guichet * guichet_covoit;

static pthread_t pid_stats;

void
Init_guichets(
);

void
Init_sync_args(
        struct Moduleur_sync_args * args
);

static
void
Init_stats(
);

void
Creer_thread_moduleur(
        pthread_t * tid,
        struct Moduleur_sync_args * args
);

_Noreturn
static
void *
thr_modulation(
        void * p_args
);

_Noreturn
static
void *
thr_stats(
        void * p_args
);

static
void
Modulation(
        struct Moduleur_sync_args * args
);

static
void
Moduleur_Ouvrir_Guichet(
        int classe,
        int num_guichet,
        int mode_paiement
);

static
void
Moduleur_Ouvrir_Guichet_Covoiturage(
);

static
void
Moduleur_Fermer_Guichet(
        int classe,
        int num_guichet
);

static
void
Moduleur_Fermer_Guichet_Covoiturage(
);

static
void
getStats(
);

#endif //PROJETA22_MODULEUR_H
