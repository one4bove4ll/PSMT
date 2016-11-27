#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "gestion_erreur.h"

#define SEMAPHORE_ID 42
#define SHM_ID 42
#define NB_ITER 1000

/* Prends le sémaphore numSem du groupe de semaphore sem */
static void P(int sem, int numSem);

/* Libère le sémaphore numSem du groupe de semaphore sem */
static void V(int sem, int numSem);

/* Retourne le temps actuel en microsecondes */
static long get_time_usec(void);

/* Procédure effectuant l'écriture dans la shm en premier */
static void first_writer(void);

/* Procédure effectuant la lecture dans la shm en premier */
static void first_reader(void);

int main(int argc, char** argv)
{
    /* Lancer le programme en first_reader en premier pour creer
     * et initialiser les sémaphores et la shm.
     * Utiliser cette commande pour lancer les 2 instances:
     * (./2_memoire_partagee 0 &) ; ./2_memoire_partagee 1 */
    ERROR_IF(argc != 2, "Le programme nécessite 1 argument "
                        "(0->first_reader, 1->first_writer)");

    switch (atoi(argv[1])) {
        case 0:
            first_reader();
            break;
        case 1:
            first_writer();
            break;
        default:
            printf("Unsupported option\n");
            exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}


static void P(int sem, int numSem)
{
    struct sembuf sop;
    sop.sem_num = numSem;   /* Numéro du sémaphore cible */
    sop.sem_flg = 0;        /* Flags par défaut */
    sop.sem_op = -1;        /* Soustrait 1 à la valeur du sémaphore */

    int err = semop(sem, &sop, 1);
    ERROR_IF(err == -1, "Erreur semop P");
}

static void V(int sem, int numSem)
{
    struct sembuf sop;
    sop.sem_num = numSem;   /* Numéro du sémaphore cible */
    sop.sem_flg = 0;        /* Flags par défaut */
    sop.sem_op = 1;         /* Ajoute 1 à la valeur du sémaphore */

    int err = semop(sem, &sop, 1);
    ERROR_IF(err == -1, "Erreur semop V");
}

static long get_time_usec(void)
{
    struct timeval tv;
    int err = gettimeofday(&tv, NULL);
    ERROR_IF(err == -1, "Erreur gettimeofday");
    return (tv.tv_sec * 1000000L) + tv.tv_usec;
}


static void first_writer(void)
{
    int err, token = 0;

    /* Récupération des 2 sémaphores */
    int sem = semget(SEMAPHORE_ID, 2, 0666);
    ERROR_IF(sem == -1, "Erreur semget");

    /* Récupération de la shm */
    int shm = shmget(SHM_ID, sizeof(int), 0666);
    ERROR_IF(shm == -1, "Erreur shmget");

    /* Récupération du pointeur sur la zone mémoire de la shm */
    int *shmInt = shmat(shm, NULL, 0);
    ERROR_IF(shmInt == (int*)-1, "Erreur shmat");

    /* Boucle de d'écriture/lecture */
    long beginning = get_time_usec();
    for (int i = 0; i < NB_ITER; i++) {
        P(sem, 0);          /* P(sem1) */
        *shmInt = token;    /* ecriture */
        V(sem, 1);          /* V(sem2); */

        P(sem, 0);          /* P(sem1) */
        token = *shmInt;    /* lecture */
        V(sem, 1);          /* V(sem2); */

        token++;
    }
    long duration = get_time_usec() - beginning;

    /* Détachement de la shm */
    err = shmdt(shmInt);
    ERROR_IF(err == -1, "Erreur shmdt");

    /* Calcul du débit */
    double rate = (2*NB_ITER*sizeof(int)*8)/(double)(duration);
    printf("Rate %f\n", rate);
}

static void first_reader(void)
{
    int err, token;

    /* Création des 2 sémaphores */
    int sem = semget(SEMAPHORE_ID, 2, 0666 | IPC_CREAT);
    ERROR_IF(sem == -1, "Erreur semget");

    /* Initialisation des semaphores */
    err = semctl(sem, 0, SETVAL, 1);        /* sem1 = 1 */
    ERROR_IF(err == -1, "Erreur semctl");
    err = semctl(sem, 1, SETVAL, 0);        /* sem2 = 0 */
    ERROR_IF(err == -1, "Erreur semctl");

    /* Création de la shm */
    int shm = shmget(SHM_ID, sizeof(int), 0666 | IPC_CREAT);
    ERROR_IF(shm == -1, "Erreur shmget");

    /* Récupération du pointeur sur la zone mémoire de la shm */
    int *shmInt = shmat(shm, NULL, 0);
    ERROR_IF(shmInt == (int*)-1, "Erreur shmat");

    /* Boucle de lecture/écriture */
    for(int i = 0; i < NB_ITER; i++) {
        P(sem, 1);          /* P(sem2) */
        token = *shmInt;    /* lecture */
        V(sem, 0);          /* V(sem1) */

        token++;

        P(sem, 1);          /* P(sem2) */
        *shmInt = token;    /* ecriture */
        V(sem, 0);          /* V(sem1) */
    }

    /* Détachement de la shm */
    err = shmdt(shmInt);
    ERROR_IF(err == -1, "Erreur shmdt");

    /* Suppression de la shm */
    err = shmctl(shm, IPC_RMID, NULL);
    ERROR_IF(err == -1, "Erreur shmctl");

    /* Suppression des sémaphores */
    err = semctl(sem, 0, IPC_RMID);
    ERROR_IF(err == -1, "Erreur semctl");
}
