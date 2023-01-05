/**
 * @author Thibaud Leclere
 * @date 22/12/22
 */

#include <stdio.h>
#include <stdlib.h>

#include "Moduleur.h"

void Init_guichets() {

    int i, j, isFirst;

    // Ouvre le premier guichet pour chaque classe et ferme les autres
    // Et initialise les files d'attente à 0
    for(i = 0; i < 5; ++i) {
        isFirst = 1;
        for(j = 0; j < NB_GUICHET_PAR_CLASSE; ++j) {
            if(isFirst) {
                Moduleur_Ouvrir_Guichet(i, j, 2);
                isFirst = 0;
            } else {
                etat_guichets[i][j] = 0;
            }
            files_d_attente[i][j] = 0;
        }
    }

    etat_guichet_covoit = 0;
}

void Init_sync_args(struct Moduleur_sync_args *args) {

    if(pthread_mutex_init(&args->mutex, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_init(&args->cond1, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_init(&args->cond2, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_init(&args->cond3, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_init(&args->cond4, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    args->compteur_voitures = 0;
    args->vehicule_courant = NULL;
}

void Init_stats(pthread_t * pid_stats, struct Moduleur_sync_args * args) {

    memset(distance_guichets, 0, sizeof(distance_guichets));
    memset(total_guichets, 0, sizeof(total_guichets));
    distance_guichet_covoit = 0;
    total_guichet_covoit = 0;

    pthread_create(pid_stats, NULL, thr_stats, args);
}

void Creer_thread_moduleur(pthread_t *tid, struct Moduleur_sync_args * args) {

    if(pthread_create(tid, NULL, thr_modulation, (void*)args) != 0)
        throw_error(__FILE__, __LINE__);
}

void * thr_modulation(void * p_args) {

    struct Moduleur_sync_args * args = (struct Moduleur_sync_args *) p_args;

    while(1) {
        if(pthread_mutex_lock(&args->mutex) != 0)
            throw_error(__FILE__, __LINE__);
        if(args->compteur_voitures == 0) {
            // S'endore en attendant un véhicule
            if(pthread_cond_wait(&args->cond1, &args->mutex) != 0)
                throw_error(__FILE__, __LINE__);
            getStats(); // On récupère les stats à chaque fois qu'un véhicule entre dans le péage
            if(pthread_mutex_unlock(&args->mutex) != 0)
                throw_error(__FILE__, __LINE__);
        } else {
            // Réveil un véhicule pour lui indiquer un péage
            if(pthread_cond_signal(&args->cond2) != 0)
                throw_error(__FILE__, __LINE__);
            // Attends que le véhicule ait envoyé ses infos
            if(pthread_cond_wait(&args->cond3, &args->mutex) != 0)
                throw_error(__FILE__, __LINE__);
            Modulation(args); // Module les guichets et en assigne un au véhicule
            if(pthread_mutex_unlock(&args->mutex) != 0)
                throw_error(__FILE__, __LINE__);
        }
    }
}

void * thr_stats(void * p_args) {

    struct Moduleur_sync_args * args = (struct Moduleur_sync_args *)p_args;

    while(1) {

        if(pthread_mutex_lock(&args->mutex) != 0)
            throw_error(__FILE__, __LINE__);

        getStats();

        if(pthread_mutex_unlock(&args->mutex) != 0)
            throw_error(__FILE__, __LINE__);

        usleep(1500000);
    }
}

void Modulation(struct Moduleur_sync_args * args) {

    // J'ai toujours le mutex du moduleur là

    if(args->total_vehicules >= SEUIL_GUICHET_COVOIT && etat_guichet_covoit == 0) {
        // Si total véhicule en periode de pointe
        // On ouvre le guichet covoit si pas déjà ouvert
        Moduleur_Ouvrir_Guichet_Covoiturage();
    } else if(args->total_vehicules < SEUIL_GUICHET_COVOIT && etat_guichet_covoit == 1) {
        //Sinon on ferme ce guichet
        Moduleur_Fermer_Guichet_Covoiturage();
    }

    // Si le vehicule est covoit et que le guichet est ouvert, il passe par le guichet covoit
    if(args->vehicule_courant->est_voiturage == 1 && etat_guichet_covoit == 1) {

        args->vehicule_courant->guichet = guichet_covoit;

    } else {

        int classe = args->vehicule_courant->classe, i, guichet_peut_etre_ouvert;
        int num_guichet_optimal, num_guichet_min, nb_file_guichet_optimal, nb_file_guichet_min;
        // On cherche où placer le véhicule
        i = 0; num_guichet_optimal = -1; num_guichet_min = -1; guichet_peut_etre_ouvert = -1;
        while(i < NB_GUICHET_PAR_CLASSE) {

            if(etat_guichets[classe][i] == 1) {
                // Le guichet est ouvert

                if(files_d_attente[classe][i] == 0 && i != 0) {
                    // Si le guichet est ouvert et que personne n'attend, on le ferme
                    Moduleur_Fermer_Guichet(classe, i);
                    // On indique que le guichet peut être ré-ouvert au cas ou
                    if(guichet_peut_etre_ouvert == -1)
                        guichet_peut_etre_ouvert = i;
                } else if(
                        files_d_attente[classe][i] < SEUIL_ALERTE
                        && (    num_guichet_optimal == -1
                                || nb_file_guichet_optimal > files_d_attente[classe][i]
                        )
                        ) {
                    // On regarde les files encore sous le seuil d'alerte, et on sauvegarde celle avec le moins de queue
                    num_guichet_optimal = i;
                    nb_file_guichet_optimal = files_d_attente[classe][i];
                    //boolBoucle = 0;
                } else if(num_guichet_min == -1 || nb_file_guichet_min > files_d_attente[classe][i]) {
                    // On regarde les files ayant dépassé le seuil d'alerte, et on sauvegarde celle avec le moins de queue
                    num_guichet_min = i;
                    nb_file_guichet_min = files_d_attente[classe][i];
                }
            } else if(guichet_peut_etre_ouvert == -1) {
                guichet_peut_etre_ouvert = i;
            }
            ++i;
        }

        if(num_guichet_optimal != -1) {
            // On envoie le vehicule vers la queue la moins remplie n'ayant pas dépassé le seuil d'alerte
            args->vehicule_courant->num_guichet = num_guichet_optimal;
        } else if(guichet_peut_etre_ouvert != -1) {
            // Si aucune file n'ayant pas dépassé le seuil d'alerte n'est disponible, on ouvre une nouvelle file si possible
            Moduleur_Ouvrir_Guichet(classe, guichet_peut_etre_ouvert, args->vehicule_courant->mode_paiement);
            args->vehicule_courant->num_guichet = guichet_peut_etre_ouvert;
        } else {
            // En dernier recours, on place les véhicules dans la file la moins remplie
            args->vehicule_courant->num_guichet = num_guichet_min;
        }

        // On informe le véhicule du guichet auquel il passera
        args->vehicule_courant->guichet = struct_guichets[classe][args->vehicule_courant->num_guichet];
    }

    // On enregistre aussi le benefice financier du guichet (qte total traité par le guichet et distance totale parcourue par les véhicules)
    if(args->vehicule_courant->guichet->classe == 5)
    {
        total_guichet_covoit++;
        distance_guichet_covoit += args->vehicule_courant->distance_parcourue;
    } else
    {
        total_guichets[args->vehicule_courant->guichet->classe][args->vehicule_courant->guichet->numero]++;
        distance_guichets[args->vehicule_courant->guichet->classe][args->vehicule_courant->guichet->numero] += args->vehicule_courant->distance_parcourue;
    }

    // On signale au véhicule que son guichet lui a été attribué
    if(pthread_cond_signal(&args->cond4) != 0)
        throw_error(__FILE__, __LINE__);
}

void Moduleur_Ouvrir_Guichet(int classe, int num_guichet, int mode_paiement) {

    etat_guichets[classe][num_guichet] = 1;

    Guichet * guichet = malloc(sizeof(*guichet));
    struct_guichets[classe][num_guichet] = guichet;
    guichet->classe = classe;
    guichet->numero = num_guichet;
    guichet->mode_paiement = mode_paiement;
    guichet->file_d_attente = &files_d_attente[classe][num_guichet];
    guichet->pid = &pid_guichets[classe][num_guichet];
    // Initialisation conditions & mutex guichet
    if(pthread_cond_init(&guichet->cond1, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_init(&guichet->cond2, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_init(&guichet->cond3, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_mutex_init(&guichet->mutex, NULL) != 0)
        throw_error(__FILE__, __LINE__);


    printf(ANSI_COLOR_CYAN "[+] Le moduleur va ouvrir le guichet %d %d" ANSI_COLOR_RESET "\n", guichet->classe, guichet->numero);
    Ouvrir_Guichet(guichet);
}

void Moduleur_Ouvrir_Guichet_Covoiturage() {

    etat_guichet_covoit = 1;

    Guichet * guichet = malloc(sizeof(*guichet));
    guichet_covoit = guichet;
    guichet->classe = 5;
    guichet->numero = 0;
    guichet->mode_paiement = 3;
    guichet->file_d_attente = &file_guichet_covoit;
    guichet->pid = &pid_guichet_coivoit;
    if(pthread_cond_init(&guichet->cond1, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_init(&guichet->cond2, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_cond_init(&guichet->cond3, NULL) != 0)
        throw_error(__FILE__, __LINE__);
    if(pthread_mutex_init(&guichet->mutex, NULL) != 0)
        throw_error(__FILE__, __LINE__);

    printf(ANSI_COLOR_CYAN "[+] Le moduleur va ouvrir le guichet de covoiturage" ANSI_COLOR_RESET "\n");
    Ouvrir_Guichet(guichet);
}

void Moduleur_Fermer_Guichet(int classe, int num_guichet) {

    etat_guichets[classe][num_guichet] = 0;

    // Liberer mémoire guichet
    free(struct_guichets[classe][num_guichet]);

    printf(ANSI_COLOR_CYAN "[-] Le moduleur va fermer le guichet %d %d" ANSI_COLOR_RESET "\n", classe, num_guichet);
    Fermer_Guichet(pid_guichets[classe][num_guichet]);
}

void Moduleur_Fermer_Guichet_Covoiturage() {

    etat_guichet_covoit = 0;

    free(guichet_covoit);

    printf(ANSI_COLOR_CYAN "[-] Le moduleur va fermer le guichet de covoiturage" ANSI_COLOR_RESET "\n");
    Fermer_Guichet(pid_guichet_coivoit);
}

void getStats() {

    int i, j;
    printf("Classe\tNum\t\tEtat\tFile\tTotal\tDistance\tBenefice\n");

    for(i = 0; i < 5; ++i) {

        for(j = 0; j < NB_GUICHET_PAR_CLASSE; ++j) {

            printf("%d\t\t%d\t\t%s\t\t%d\t\t%d\t\t%f\t%f\n",
                   i,
                   j,
                   (etat_guichets[i][j] == 1 ? (ANSI_COLOR_GREEN "o" ANSI_COLOR_RESET) : (ANSI_COLOR_RED "x" ANSI_COLOR_RESET)),
                   files_d_attente[i][j],
                   total_guichets[i][j],
                   distance_guichets[i][j],
                   distance_guichets[i][j] / (total_guichets[i][j] == 0 ? 1 : total_guichets[i][j]));
        }
    }
    printf("5\t\t0\t\t%s\t\t%d\t\t%d\t\t%f\t%f\n\n",
           (etat_guichet_covoit == 1 ? (ANSI_COLOR_GREEN "o" ANSI_COLOR_RESET) : (ANSI_COLOR_RED "x" ANSI_COLOR_RESET)),
           file_guichet_covoit,
           total_guichet_covoit,
           distance_guichet_covoit,
           distance_guichet_covoit / (total_guichet_covoit == 0 ? 1 : total_guichet_covoit));
}
