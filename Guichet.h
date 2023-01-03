//
// Created by thibaud on 23/12/22.
//

#ifndef PROJETA22_GUICHET_H
#define PROJETA22_GUICHET_H

#include "Colors.h"
#include "Peage_utils.h"

#include <string.h>
#include <stdio.h>


_Noreturn
static
void *
thr_guichet(
        void * p_args
);

void
Ouvrir_Guichet(
        Guichet * guichet
);

void
Fermer_Guichet(
        pthread_t pid
);

void
Ouvrir_Guichet_Covoit(
        Guichet * guichet
);

void
Fermer_Guichet_Covoit(
        pthread_t pid
);


#endif //PROJETA22_GUICHET_H
