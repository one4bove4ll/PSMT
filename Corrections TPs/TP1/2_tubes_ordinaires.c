#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "gestion_erreur.h"

#define NB_ITER 1000

/* Retourne un entier lu dans le pipe passé en paramètre */
static int read_pipe(int pipe[]);

/* Ecrit la valeur toSend dans le pipe passé en paramètre */
static void write_pipe(int pipe[], int toSend);

/* Retourne le temps actuel en microsecondes */
static long get_time_usec(void);

int main(int argc, char** argv)
{
    int err;

    /* Le token que nous allons envoyer par les pipes */
    int token = 0;

    /* Création des pipes */
    int childPipe[2], parentPipe[2];    /* pipe[0]=lecture, pipe[1]=ecriture */
    err = pipe(childPipe);              /* communication fils -> père */
    ERROR_IF(err == -1, "Erreur pipe");
    err = pipe(parentPipe);             /* communication père -> fils */
    ERROR_IF(err == -1, "Erreur pipe");

    /* Création du processus fils */
    pid_t pid = fork();
    ERROR_IF(pid == -1, "Erreur fork");

    if(pid == 0) {  /* Processus fils */
        printf("Child process id : %d\n", getpid());
        /* On ferme les descripteurs de communication que l'on utilise pas */
        err = close(childPipe[1]);
        ERROR_IF(err == -1, "Erreur close");
        err = close(parentPipe[0]);
        ERROR_IF(err == -1, "Erreur close");

        /* Boucle de lecture/écriture */
        for(int i = 0; i < NB_ITER; i++) {
            token = read_pipe(childPipe);
            token++;
            write_pipe(parentPipe, token);
        }

        /* Fermeture des descripteurs de communication restant */
        err = close(childPipe[0]);
        ERROR_IF(err == -1, "Erreur close");
        err = close(parentPipe[1]);
        ERROR_IF(err == -1, "Erreur close");
    } else {        /* Processus père */
        printf("Parent process id : %d\n",getpid());
        /* On ferme les descripteurs de communication que l'on utilise pas */
        err = close(childPipe[0]);
        ERROR_IF(err == -1, "Erreur close");
        err = close(parentPipe[1]);
        ERROR_IF(err == -1, "Erreur close");

        /* Boucle de d'écriture/lecture */
        long beginning = get_time_usec();
        for (int i = 0; i < NB_ITER; i++) {
            write_pipe(childPipe, token);
            token = read_pipe(parentPipe);
            token++;
        }
        long duration = get_time_usec() - beginning;

        /* Fermeture des descripteurs de communication restant */
        err = close(childPipe[1]);
        ERROR_IF(err == -1, "Erreur close");
        err = close(parentPipe[0]);
        ERROR_IF(err == -1, "Erreur close");

        /* Calcul du débit */
        double rate = (2*NB_ITER*sizeof(int)*8)/(double)(duration);
        printf("Rate %f\n", rate);
    }

    return EXIT_SUCCESS;
}

static int read_pipe(int pipe[])
{
    int buffer;
    int err = read(pipe[0], &buffer, sizeof(int));
    ERROR_IF(err == -1, "Erreur read");
    return buffer;
}

static void write_pipe(int pipe[], int toSend)
{
    int err = write(pipe[1], &toSend, sizeof(int));
    ERROR_IF(err == -1, "Erreur write");
}

static long get_time_usec(void)
{
    struct timeval tv;
    int err = gettimeofday(&tv, NULL);
    ERROR_IF(err == -1, "Erreur gettimeofday");
    return (tv.tv_sec * 1000000L) + tv.tv_usec;
}
