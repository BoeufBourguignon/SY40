//
// Created by thibaud on 23/12/22.
//
#include "Guichet.h"

void * thr_guichet(void * p_args) {

    Guichet * guichet = (Guichet*)p_args;

    while(1) {
        if(pthread_mutex_lock(&guichet->mutex) != 0)
            throw_error(__FILE__, __LINE__);
        if(*(guichet->file_d_attente) == 0) {
            if(pthread_cond_wait(&guichet->cond1, &guichet->mutex) != 0)
                throw_error(__FILE__, __LINE__);
            if(pthread_mutex_unlock(&guichet->mutex) != 0)
                throw_error(__FILE__, __LINE__);
        } else {
            if(pthread_cond_signal(&guichet->cond2) != 0)
                throw_error(__FILE__, __LINE__);
            if(pthread_cond_wait(&guichet->cond3, &guichet->mutex) != 0)
                throw_error(__FILE__, __LINE__);
            if(pthread_mutex_unlock(&guichet->mutex) != 0)
                throw_error(__FILE__, __LINE__);
        }
    }
}

void Ouvrir_Guichet(Guichet * guichet) {

    if(pthread_create(guichet->pid, NULL, thr_guichet, (void*)guichet) != 0)
        throw_error(__FILE__, __LINE__);
}

void Fermer_Guichet(pthread_t pid) {

    if(pthread_cancel(pid) != 0)
        throw_error(__FILE__, __LINE__);
}

void Ouvrir_Guichet_Covoit(Guichet * guichet) {

    if(pthread_create(guichet->pid, NULL, thr_guichet, (void*)guichet) != 0)
        throw_error(__FILE__, __LINE__);
}

void Fermer_Guichet_Covoit(pthread_t pid) {

    if(pthread_cancel(pid) != 0)
        throw_error(__FILE__, __LINE__);
}
