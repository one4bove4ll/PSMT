#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);


int incrementation(void)
{
    /* Utilisation d'une variable statique */
    static int value = 0;
    return ++value;
}

void display_value(int *n)
{
    for(int i = 0; i < *n; i++) {
        printf("Value = %d\n", incrementation());
    }
}

int main(int argc, char** argv)
{
    const int nb_threads = 2;
    pthread_t threads[nb_threads];
    int err, n = 10;

    /* Lancement des threads */
    for (int i = 0; i < nb_threads; i++) {
        err = pthread_create(&threads[i], NULL, (fct_ptr_t)display_value, &n);
        ERROR_IF(err != 0, "Erreur pthread_create");
    }

    display_value(&n);

    /* Attente de la terminaison des threads */
    for (int i = 0; i < nb_threads; i++) {
        err = pthread_join(threads[i], NULL);
        ERROR_IF(err != 0, "Erreur pthread_join");
    }

    return EXIT_SUCCESS;
}
