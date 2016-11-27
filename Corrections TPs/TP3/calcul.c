#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "gestion_erreur.h"

#define VECT_SIZE 1000
#define VECT_RAND_MAX 100
#define MIN(a,b) ((a) < (b))? (a) : (b)

/*
 * Déclarations des types
 */

/* Type pointeur de fonction */
typedef void *(*fct_ptr_t)(void *);
/* Définition des deux méthodes */
enum method {
    ROW_METHOD = 1,
    COLUMN_METHOD = 2
};
/* Les paramètres passés aux threads */
struct thread_params {
    enum method m;
    int index;
    uint *vector;
    uint (*matrix)[VECT_SIZE];
};


/*
 * Déclaration des fonctions locales
 */

/* Retourne le temps actuel en microsecondes */
static long get_time_usec(void);

/* Fonction associée à un thread pour effectuer le calcul demandé */
void calcul_thread(struct thread_params *p);


/*
 * Corps des fonction locales
 */

int main(int argc, char** argv)
{
    /* Récupération et vérification des arguments */
    ERROR_IF(argc != 4, "Usage: ./calcul method nbThreads nbIter");
    enum method m = atoi(argv[1]);
    ERROR_IF(m != COLUMN_METHOD && m != ROW_METHOD, "Methode incorrecte");
    int nbThreads = atoi(argv[2]);
    ERROR_IF(nbThreads < 1, "nbThreads doit être positif");
    int nbIter = atoi(argv[3]);
    ERROR_IF(nbIter < 1, "nbIter doit être positif");

    /* Définition et initialisation des vecteurs et matrices */
    uint matrix[VECT_SIZE][VECT_SIZE];
    uint vector[VECT_SIZE];
    uint v_result[VECT_SIZE];

    srand(time(NULL));
    for(int i = 0; i < VECT_SIZE; i++) {
        vector[i] = rand() % VECT_RAND_MAX;
        for(int j = 0; j < VECT_SIZE; j++) {
            matrix[i][j] = rand() % VECT_RAND_MAX;
        }
    }

    /* Définition et initialisation des infos pour les threads */
    pthread_t tids[nbThreads];
    struct thread_params tparams[nbThreads];

    for(int i = 0; i < nbThreads; i++) {
        tparams[i].m = m;
        tparams[i].vector = vector;
        tparams[i].matrix = matrix;
    }

    /* Exécution des calculs multithreadés */
    long startTime = get_time_usec();
    for(int iter = 0; iter < nbIter; iter++) {
        for(int n = 0; n < VECT_SIZE; n += nbThreads) {
            /* On ajuste le nombre de threads à lancer
             * (Cas où VECT_SIZE n'est pas multiple de nbThreads) */
            int realNbThreads = MIN(VECT_SIZE - n*nbThreads, nbThreads);
            /* Lancement des threads */
            for(int i = 0; i < realNbThreads; i++) {
                tparams[i].index = n+i;
                int err = pthread_create(&tids[i], NULL,
                                        (fct_ptr_t)calcul_thread, &tparams[i]);
                ERROR_IF(err != 0, "Erreur pthread_create");
            }
            /* Récupération des résultats */
            for(int i = 0; i < realNbThreads; i++) {
                uint *result;
                int err = pthread_join(tids[i], (void**)&result);
                ERROR_IF(err != 0, "Erreur pthread_join");
                v_result[n+i] = *result;
                free(result);   /* result à été malloc dans le thread */
            }
        }
        /* A l'itération suivante on prends le résultat comme nouvelle entrée */
        memcpy(vector, v_result, VECT_SIZE * sizeof(uint));
    }
    long duration = get_time_usec() - startTime;

    /* Ecriture des paramètres et du temps d'exécution dans le fichier log */
    FILE* logFile = fopen("calcul.log", "a");
    ERROR_IF(logFile == NULL, "Erreur fopen");

    fprintf(logFile, "Méthode = %d, nombre de thread = %d, "
                     "nombre d'iteration = %d, temps d'execution = %ldms\n",
                     m, nbThreads, nbIter, duration/1000L);

    int err = fclose(logFile);
    ERROR_IF(err != 0, "Erreur fclose");

    return EXIT_SUCCESS;
}


static long get_time_usec(void)
{
    struct timeval tv;
    int err = gettimeofday(&tv, NULL);
    ERROR_IF(err == -1, "Erreur gettimeofday");
    return (tv.tv_sec * 1000000L) + tv.tv_usec;
}

void calcul_thread(struct thread_params *p)
{
    /* Allocation et initialisation du résultat */
    uint *result = malloc(sizeof(uint));
    ERROR_IF(result == NULL, "Erreur malloc");
    *result = 0;

    /* Calcul en fonction de la méthode */
    if(p->m == ROW_METHOD) {
        for (int i = 0; i < VECT_SIZE; i++) {
            *result += p->matrix[p->index][i] * p->vector[i];
        }

    } else {
        for (int i = 0; i < VECT_SIZE; i++) {
            *result += p->matrix[i][p->index] * p->vector[i];
        }
    }

    /* Renvoi du résultat au main */
    pthread_exit((void*)result);
}
