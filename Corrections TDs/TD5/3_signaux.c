#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);

void handler(int signum)
{
    if(signum == SIGUSR2)
        exit(EXIT_SUCCESS);

    printf("Reception du signal %d dans le thread %lu\n",
            signum, pthread_self());
}

void fonction_thread1(void)
{
    /* Définition des signaux à bloquer ou non */
    int err;
    sigset_t sig;
    err = sigfillset(&sig);         /* Tous les signaux bloqués par défaut */
    ERROR_IF(err == -1, "Erreur sigfillset");
    err = sigdelset(&sig, SIGUSR1); /* Sauf SIGUSR1 */
    ERROR_IF(err == -1, "Erreur sigdelset");

    /* Application du blocage des signaux */
    err = pthread_sigmask(SIG_SETMASK, &sig, NULL);
    ERROR_IF(err != 0, "Erreur pthread_sigmask");

    /* Définition du signal handler pour SIGUSR1 */
    struct sigaction exit_action;
    exit_action.sa_handler = handler;   /* Handler pour SIGUSR1 */
    exit_action.sa_mask = sig;  /* Signaux bloqués pendant l'exec. du handler */
    exit_action.sa_flags = 0;           /* Flags par défaut */
    err = sigaction(SIGUSR1, &exit_action, NULL);
    ERROR_IF(err == -1, "Erreur sigaction");

    /* Boucle d'attente passive */
    while(1) {
        pause();    /* Sleep jusqu'à la réception d'un signal */
    }
}


int main()
{
    /* Affichage du pid (necessaire pour le kill depuis le terminal) */
    printf("pid=%d\n", getpid());

    /* Creation du thread */
    pthread_t tid;
    int err = pthread_create(&tid, NULL, (fct_ptr_t)fonction_thread1, NULL);
    ERROR_IF(err != 0, "Erreur pthread_create");

    /* Définition des signaux à bloquer ou non */
    sigset_t sig;
    err = sigfillset(&sig);         /* Tous les signaux bloqués par défaut */
    ERROR_IF(err == -1, "Erreur sigfillset");
    err = sigdelset(&sig, SIGUSR2); /* sauf SIGUSR2 */
    ERROR_IF(err == -1, "Erreur sigdelset");

    /* Application du blocage des signaux */
    err = pthread_sigmask(SIG_SETMASK, &sig, NULL);
    ERROR_IF(err != 0, "Erreur pthread_sigmask");

    /* Définition du signal handler pour SIGUSR2 */
    struct sigaction exit_action;
    exit_action.sa_handler = handler;   /* Handler pour SIGUSR2 */
    exit_action.sa_mask = sig;  /* Signaux bloqués pendant l'exec. du handler */
    exit_action.sa_flags = 0;           /* Flags par défaut */
    err = sigaction(SIGUSR2, &exit_action, NULL);
    ERROR_IF(err == -1, "Erreur sigaction");


    /* Boucle d'attente passive */
    while(1) {
        pause();    /* Sleep jusqu'à la réception d'un signal */
    }

    return EXIT_SUCCESS;
}
