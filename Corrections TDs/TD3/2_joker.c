#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);

static float *px = NULL;
static float *py = NULL;

/* Initialisation des mutexes */
static pthread_mutex_t mutex_px = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_py = PTHREAD_MUTEX_INITIALIZER;

/* Initialisation de la condition */
static pthread_cond_t cond_py = PTHREAD_COND_INITIALIZER;

void print_px(int *n)
{
    int err;
    for(int i=0; i < *n; i++) {
        /* Protection de la section critique associée à py */
        err = pthread_mutex_lock(&mutex_py);
        ERROR_IF(err != 0, "Erreur pthread_mutex_lock");

        printf("py=%f\n", *py);

        /* On indique à l'autre thread qu'on va libérer py */
        err = pthread_cond_signal(&cond_py);
        ERROR_IF(err != 0, "Erreur pthread_cond_signal");
        err = pthread_mutex_unlock(&mutex_py);
        ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");
    }
}

void print_px_py(int *n)
{
    int err;
    for(int i=0; i < *n; i++) {
        /* Appel bloquant pour aquérir mutex_px */
        err = pthread_mutex_lock(&mutex_px);
        ERROR_IF(err != 0, "Erreur pthread_mutex_lock");

        /* Boucle d'attente passive pour aquérir mutex_py */
        err = pthread_mutex_trylock(&mutex_py);
        while(err == EBUSY) {
            /* on libère mutex_px si on obtiens pas mutex_py
             * et on attends le signal de la condition */
            err = pthread_cond_wait(&cond_py, &mutex_px);
            ERROR_IF(err != 0, "Erreur pthread_cond_wait");

            err = pthread_mutex_trylock(&mutex_py);
        }
        ERROR_IF(err != 0, "Erreur pthread_mutex_trylock");

        /* Section critique associée à px et py */
        printf("px=%f, py=%f\n", *px, *py);

        /* Libération des mutexes */
        err = pthread_mutex_unlock(&mutex_py);
        ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");
        err = pthread_mutex_unlock(&mutex_px);
        ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");
    }
}

int main (int argc, char** argv)
{
    pthread_t thread2, thread3;
    float x = 1.0;
    float y = 42.0;
    int max, err;

    ERROR_IF(argc != 2, "Précise le nombre d'occurences de la boucle");
    max = atoi(argv[1]);
    px = &x;
    py = &y;

    err = pthread_create(&thread2, NULL, (fct_ptr_t)print_px, &max);
    ERROR_IF(err != 0, "Erreur pthread_create");
    err = pthread_create(&thread3, NULL, (fct_ptr_t)print_px_py, &max);
    ERROR_IF(err != 0, "Erreur pthread_create");

    for(int i=0; i < max; i++) {
        /* Protection de la section critique associée à px */
        err = pthread_mutex_lock(&mutex_px);
        ERROR_IF(err != 0, "Erreur pthread_mutex_lock");

        px = NULL;
        printf("i=%d\n", i);
        px = &x;

        err = pthread_mutex_unlock(&mutex_px);
        ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");
    }

    err = pthread_join(thread2, NULL);
    ERROR_IF(err != 0, "Erreur pthread_join");
    err = pthread_join(thread3, NULL);
    ERROR_IF(err != 0, "Erreur pthread_join");

    return EXIT_SUCCESS;
}
