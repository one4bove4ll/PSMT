#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gestion_erreur.h"

#define NB_ITER 1000

/* Retourne un entier lu dans le decripteur passé en paramètre */
static int read_pipe(int fd);

/* Ecrit la valeur toSend dans le decripteur passé en paramètre */
static void write_pipe(int fd, int toSend);

/* Retourne le temps actuel en microsecondes */
static long get_time_usec(void);

/* Procédure effectuant l'écriture dans le pipe en premier */
static void first_writer(void);

/* Procédure effectuant la lecture dans le pipe en premier */
static void first_reader(void);


int main(int argc, char** argv)
{
    /* Lancer le programme en first_reader en premier pour creer les fifos.
     * Utiliser cette commande pour lancer les 2 instances:
     * (./3_tubes_nommes 0 &) ; ./3_tubes_nommes 1 */
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


static int read_pipe(int fd)
{
    int buffer;
    int err = read(fd, &buffer, sizeof(int));
    ERROR_IF(err == -1, "Erreur read");
    return buffer;
}

static void write_pipe(int fd, int toSend)
{
    int err = write(fd, &toSend, sizeof(int));
    ERROR_IF(err == -1, "Erreur write");
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

    /* Ouverture des fifos */
    int fdWrite = open("fifo1", O_WRONLY);
    ERROR_IF(fdWrite == -1, "Erreur open");
    int fdRead = open("fifo2", O_RDONLY);
    ERROR_IF(fdRead == -1, "Erreur open");

    /* Boucle de d'écriture/lecture */
    long beginning = get_time_usec();
    for (int i = 0; i < NB_ITER; i++) {
        write_pipe(fdWrite, token);
        token = read_pipe(fdRead);
        token++;
    }
    long duration = get_time_usec() - beginning;

    /* Fermeture des descripteurs de communication */
    err = close(fdRead);
    ERROR_IF(err == -1, "Erreur close");
    err = close(fdWrite);
    ERROR_IF(err == -1, "Erreur close");

    /* Calcul du débit */
    double rate = (2*NB_ITER*sizeof(int)*8)/(double)(duration);
    printf("Rate %f\n", rate);
}

static void first_reader(void)
{
    int err, token;

    /* Création des fifos */
    err = mkfifo ("fifo1", S_IRUSR|S_IWUSR|S_IRGRP);
    ERROR_IF(err == -1, "Erreur mkfifo");
    err = mkfifo ("fifo2", S_IRUSR|S_IWUSR|S_IRGRP);
    ERROR_IF(err == -1, "Erreur mkfifo");

    /* Ouverture des fifos précédemment créés */
    int fdRead = open("fifo1", O_RDONLY);
    ERROR_IF(fdRead == -1, "Erreur open");
    int fdWrite = open("fifo2", O_WRONLY);
    ERROR_IF(fdWrite == -1, "Erreur open");

    /* Boucle de lecture/écriture */
    for(int i = 0; i < NB_ITER; i++) {
        token = read_pipe(fdRead);
        token++;
        write_pipe(fdWrite, token);
    }

    /* Fermeture des descripteurs de communication */
    err = close(fdRead);
    ERROR_IF(err == -1, "Erreur close");
    err = close(fdWrite);
    ERROR_IF(err == -1, "Erreur close");

    /* Suppression des fifos */
    err = unlink("fifo1");
    ERROR_IF(err == -1, "Erreur unlink");
    err = unlink("fifo2");
    ERROR_IF(err == -1, "Erreur unlink");
}
