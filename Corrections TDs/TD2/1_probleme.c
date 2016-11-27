#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);

static float *px = NULL;

void print_px(int *max)
{
    for(int i=0; i < *max; i++) {
        /* Possible déréférencement de pointeur NULL
         * quand px est modifié dans la boucle principale */
        printf("px = %f\n", *px);
    }
}

int main(int argc, char** argv)
{
    pthread_t t_id;
    int max;
    float x = 1.0;

    ERROR_IF(argc != 2, "Précise le nombre d'occurences de la boucle");
    max = atoi(argv[1]);
    px = &x;

    int err = pthread_create(&t_id, NULL, (fct_ptr_t)print_px, &max);
    ERROR_IF(err != 0, "Erreur pthread_create");

    for(int i=0; i < max; i++) {
        /* Modification de px */
        px = NULL;
        printf("i = %d\n",i);
        px = &x;
    }

    err = pthread_join(t_id, NULL);
    ERROR_IF(err != 0, "Erreur pthread_join");

    return EXIT_SUCCESS;
}
