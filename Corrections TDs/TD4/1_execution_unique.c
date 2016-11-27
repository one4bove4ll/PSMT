#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);

/* Initialisation du "once control" */
static pthread_once_t fonction_init_once = PTHREAD_ONCE_INIT;

void fonction_init(void)
{
    pthread_t t_id = pthread_self();
    printf("Init function called from thread %lu\n", t_id);
}

void say_hello(int *n)
{
    /* On appelle la fonction une seule fois */
    int err = pthread_once(&fonction_init_once, fonction_init);
    ERROR_IF(err != 0, "Erreur pthread_once");

    pthread_t t_id = pthread_self();

    for(int i = 0; i < *n; i++) {
        printf("Hello from thread %lu\n", t_id);
        sleep(1);
    }
}

int main(int argc, char** argv)
{
    const int nb_threads = 2;
    pthread_t threads[nb_threads];
    int err, n = 2;

    /* Lancement des threads */
    for (int i = 0; i < nb_threads; i++) {
        err = pthread_create(&threads[i], NULL, (fct_ptr_t)say_hello, &n);
        ERROR_IF(err != 0, "Erreur pthread_create");
    }

    say_hello(&n);

    /* Attente de la terminaison des threads */
    for (int i = 0; i < nb_threads; i++) {
        err = pthread_join(threads[i], NULL);
        ERROR_IF(err != 0, "Erreur pthread_join");
    }

    return EXIT_SUCCESS;
}
