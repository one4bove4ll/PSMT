#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);

static float *px = NULL;

/* Initialisation du mutex */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void print_px(int *max)
{
    int err;
    for(int i=0; i < *max; i++) {
        /* Protection de la section critique */
        err = pthread_mutex_lock(&mutex);
        ERROR_IF(err != 0, "Erreur pthread_mutex_lock");

        printf("px = %f\n", *px);

        err = pthread_mutex_unlock(&mutex);
        ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");
    }
}

int main(int argc, char** argv)
{
    pthread_t t_id;
    int max, err;
    float x = 1.0;

    ERROR_IF(argc != 2, "Précise le nombre d'occurences de la boucle");
    max = atoi(argv[1]);
    px = &x;

    err = pthread_create(&t_id, NULL, (fct_ptr_t)print_px, &max);
    ERROR_IF(err != 0, "Erreur pthread_create");

    for(int i=0; i < max; i++) {
        /* Protection de la section critique */
        err = pthread_mutex_lock(&mutex);
        ERROR_IF(err != 0, "Erreur pthread_mutex_lock");

        px = NULL;
        printf("i = %d\n",i);
        px = &x;

        err = pthread_mutex_unlock(&mutex);
        ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");
    }

    err = pthread_join(t_id, NULL);
    ERROR_IF(err != 0, "Erreur pthread_join");

    return EXIT_SUCCESS;
}
