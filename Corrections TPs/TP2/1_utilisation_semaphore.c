#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <errno.h>
#include "gestion_erreur.h"

#define SEMAPHORE_ID 42
#define BUFFER_SIZE 20

/* Prends le sémaphore sem */
static void P(int sem);

/* Libère le sémaphore sem */
static void V(int sem);


int main(int argc, char** argv)
{
    int err;

    /* Récupération du sémaphore */
    int sem = semget(SEMAPHORE_ID, 1, 0666);
    if(sem == -1) {
        /* On sort sur erreur si l'instruction précédente à échouée
         * pour une autre raison que la non existance du sémaphore */
        ERROR_IF(errno != ENOENT, "Erreur semget");
        /* Donc si il n'existe pas déjà on le crée */
        sem = semget(SEMAPHORE_ID, 1, 0666 | IPC_CREAT);
        ERROR_IF(sem == -1, "Erreur semget create");
        /* Initialisation de sa valeur à 1 */
        err = semctl(sem, 0, SETVAL, 1);
        ERROR_IF(err == -1, "Erreur semctl init");
    }

    printf("/!\\ Le programme ne supprime pas le sémaphore à la fin de "
           "l'exécution, n'oubliez pas de le supprimer manuellement "
           "avec la commande \"ipcrm -S %d\"\n", SEMAPHORE_ID);

    char buffer[BUFFER_SIZE];
    do {
        P(sem);

        /* Section à protéger */
        printf("Entrez une chaine de caractere (exit pour quitter) : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        strtok(buffer, "\n");           /* Suppression du '\n' */
        printf("Chaîne lue = \"%s\"\n", buffer);

        V(sem);
    } while(strncmp(buffer, "exit", BUFFER_SIZE) != 0);

    return EXIT_SUCCESS;
}


static void P(int sem)
{
    struct sembuf sop;
    sop.sem_num = 0;    /* Premier sémaphore du groupe */
    sop.sem_flg = 0;    /* Flags par défaut */
    sop.sem_op = -1;    /* Soustrait 1 à la valeur du sémaphore */

    int err = semop(sem, &sop, 1);
    ERROR_IF(err == -1, "Erreur semop P");
}

static void V(int sem)
{
    struct sembuf sop;
    sop.sem_num = 0;    /* Premier sémaphore du groupe */
    sop.sem_flg = 0;    /* Flags par défaut */
    sop.sem_op = 1;     /* Ajoute 1 à la valeur du sémaphore */

    int err = semop(sem, &sop, 1);
    ERROR_IF(err == -1, "Erreur semop V");
}
