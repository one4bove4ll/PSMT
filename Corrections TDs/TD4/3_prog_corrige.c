#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);

static pthread_once_t fonction_init_once = PTHREAD_ONCE_INIT;
static pthread_key_t key;

void fonction_init()
{
    /* Initialisation de la clé, free sera appelé pour libérer
     * les ressources associées à cette clé lors de la fin du thread */
    int err = pthread_key_create(&key, free);
    ERROR_IF(err != 0, "Erreur pthread_key_create");
}

int incrementation()
{
    /* Incrémentation de la variable spécifique */
    int *x = (int*)pthread_getspecific(key);
    ERROR_IF(x == NULL, "Erreur: aucune variable spécifique allouée");
    return ++(*x);
}

void display_value(int *n)
{
    /* Appel unique à la fonction init */
    int err = pthread_once(&fonction_init_once, fonction_init);
    ERROR_IF(err != 0, "Erreur pthread_once");

    /* Initialisation de la variable spécifique au thread */
    int *buf = malloc(sizeof(int));
    ERROR_IF(buf == NULL, "Erreur malloc");
    *buf = pthread_self() % INT_MAX;    /* pour eviter l'overflow */
    err = pthread_setspecific(key, buf);
    ERROR_IF(err != 0, "Erreur pthread_setspecific");

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

    /* Nécessaire pour que la fonction free soit appelée
     * sur la variable spécifique */
    pthread_exit(NULL);

    return EXIT_SUCCESS;
}
