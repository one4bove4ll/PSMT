#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);

static pthread_mutex_t mutex_ressources = PTHREAD_MUTEX_INITIALIZER;
static unsigned int nb_ressources = 4;

void f_thread(unsigned int *ressources_necessaires)
{
    int err;
    pthread_t current_thread = pthread_self();

    /*
     * Attente des ressources
     */
    err = pthread_mutex_lock(&mutex_ressources);
    ERROR_IF(err != 0, "Erreur pthread_mutex_lock");

    /* Boucle d'attente active */
    while (nb_ressources < *ressources_necessaires) {
        err = pthread_mutex_unlock(&mutex_ressources);
        ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");

        err = pthread_mutex_lock(&mutex_ressources);
        ERROR_IF(err != 0, "Erreur pthread_mutex_lock");
    }

    /*
     * Prise des ressources
     */
    printf("*** Thread[%lu] ***\nJe prends %u ressources",
           current_thread,
           *ressources_necessaires);
    nb_ressources -= *ressources_necessaires;
    printf(" donc il reste %u ressources\n\n", nb_ressources);

    err = pthread_mutex_unlock(&mutex_ressources);
    ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");

    /*
     * Algorithme complexe nécessitant les ressources
     */
    sleep(*ressources_necessaires);

    /*
     * Libération des ressources
     */
    err = pthread_mutex_lock(&mutex_ressources);
    ERROR_IF(err != 0, "Erreur pthread_mutex_lock");

    printf("*** Thread[%lu] ***\nJe libère %u ressources",
           current_thread,
           *ressources_necessaires);
    nb_ressources += *ressources_necessaires;
    printf(" donc il reste %u ressources\n\n", nb_ressources);

    err = pthread_mutex_unlock(&mutex_ressources);
    ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");
}

int main(int argc, char** argcv)
{
    pthread_t threads[3];
    unsigned int args[3] = {3, 2, 1};
    int err;

    /* Lancement des 3 threads */
    for (int i = 0; i < 3; i++) {
        err = pthread_create(&threads[i], NULL, (fct_ptr_t)f_thread, &args[i]);
        ERROR_IF(err != 0, "Erreur pthread_create");
    }

    /* Attente des threads */
    for (int i = 0; i < 3; i++) {
        err = pthread_join(threads[i], NULL);
        ERROR_IF(err != 0, "Erreur pthread_join");
    }

    return EXIT_SUCCESS;
}
