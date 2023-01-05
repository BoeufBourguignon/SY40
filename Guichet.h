/**
 * @author Thibaud Leclere
 * @date 23/12/22
 */

#ifndef PROJETA22_GUICHET_H
#define PROJETA22_GUICHET_H

#include "Colors.h"
#include "Peage_utils.h"

#include <string.h>
#include <stdio.h>

/**
 * Thread d'un guichet, qui attend un véhicule et lui autorise à passer
 * @param p_args
 * @return
 */
_Noreturn
static
void *
thr_guichet(
        void * p_args
);

/**
 * Crée un thread pour un certain guichet
 * @param guichet
 */
void
Ouvrir_Guichet(
        Guichet * guichet
);

/**
 * Ferme un thread d'un guichet, selon le pid de ce thread
 * @param pid
 */
void
Fermer_Guichet(
        pthread_t pid
);

/**
 * Ouvre le thread du guichet de covoiturage
 * @param guichet
 */
void
Ouvrir_Guichet_Covoit(
        Guichet * guichet
);

/**
 * Ferme le thread du guichet de covoiturage
 * @param pid
 */
void
Fermer_Guichet_Covoit(
        pthread_t pid
);


#endif //PROJETA22_GUICHET_H
