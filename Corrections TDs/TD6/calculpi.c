#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "gestion_erreur.h"

#define DIAMETRE_CERCLE 5000
#define RAYON_CERCLE (DIAMETRE_CERCLE/2)
#define RAYON_CERCLE_2 (RAYON_CERCLE * RAYON_CERCLE)

typedef void *(*fct_ptr_t)(void *);

static void lance_fleche(const int *nb_fleches);
static long get_time_ms(void);

int main(int argc, char **argv)
{
    ERROR_IF(argc != 4, "Usage: ./calculpi nb_threads nb_fleches nom_fichier");

    int nb_threads = atoi(argv[1]);
    int nb_fleches = atoi(argv[2]);
    char *nom_fichier = argv[3];
    int err;

    ERROR_IF((nb_fleches % nb_threads) != 0,
             "nb_fleches doit être divisible par nb_threads");

	long debut = get_time_ms();     /* Début */

    /* Allocation des tableaux de tid et lancement des threads */
    pthread_t *thread_pool = calloc(nb_threads, sizeof(pthread_t));
    ERROR_IF(thread_pool == NULL, "Erreur calloc");
    int nb_fleches_par_threads = nb_fleches / nb_threads;
    for(int i=0; i<nb_threads; ++i) {
        err = pthread_create(&(thread_pool[i]), NULL, (fct_ptr_t)lance_fleche,
                             &nb_fleches_par_threads);
        ERROR_IF(err != 0, "Erreur pthread_create");
    }

    /* Récupération des résultats */
    int resultat = 0;
    for(int i=0; i<nb_threads; ++i) {
        int *resultat_thread;
        err = pthread_join(thread_pool[i], (void**)(&resultat_thread));
        ERROR_IF(err != 0, "Erreur pthread_join");
        resultat += *resultat_thread;
        free(resultat_thread);
    }
    free(thread_pool);

    long fin = get_time_ms();       /* Fin */

    /* Ecriture des résultats dans le fichier */
    FILE *fd = fopen(nom_fichier, "w");
    ERROR_IF(fd == NULL, "Erreur fopen");

    fprintf(fd, "PI=%f\n", 4 * ((double)resultat / (double)nb_fleches));
    fprintf(fd, "Temps écoulé %ldms\n", fin-debut);

    err = fclose(fd);
    ERROR_IF(err != 0, "Erreur fclose");

    return EXIT_SUCCESS;
}

static void lance_fleche(const int *nb_fleches)
{
    unsigned int seed = pthread_self();
    int *result = malloc(sizeof(int));
    ERROR_IF(result == NULL, "Erreur malloc");
    *result = 0;

    for (int i=0; i<*nb_fleches; ++i) {
        int x = rand_r(&seed) % (2*RAYON_CERCLE) - RAYON_CERCLE;
        int y = rand_r(&seed) % (2*RAYON_CERCLE) - RAYON_CERCLE;

        if((x*x + y*y) < RAYON_CERCLE_2) {
            ++(*result);
        }
    }

    pthread_exit(result);
}

static long get_time_ms(void)
{
    int err;
    struct timeval tv;
	err = gettimeofday(&tv, NULL);
    ERROR_IF(err == -1, "Erreur gettimeofday");
	return((tv.tv_sec) * 1000L + (tv.tv_usec) / 1000L);
}
