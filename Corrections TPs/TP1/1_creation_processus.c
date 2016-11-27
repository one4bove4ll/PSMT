#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "gestion_erreur.h"

/* Masque tous les signaux dans le processus courant */
static void mask_all_signals(void);

/* Démasque le signal sigNum */
static void unmask_signal(int sigNum);

/* Affecte la fonction handler à la gestion du signal sigNum */
static void bind_signal(int sigNum, void(*handler)(int));

/* Notre handler de signaux */
static void sig_handler(int sigNum);

int main(int argc, char** argv)
{
    /* Activation du signal SIGUSR1 */
    mask_all_signals();
    unmask_signal(SIGUSR1);
    bind_signal(SIGUSR1, sig_handler);

    /* Création du processus fils */
    pid_t pid = fork();
    ERROR_IF(pid == -1, "Erreur fork");

    if(pid == 0) {  /* Child process */
        printf("Child process id : %d\n", getpid());

        /* Boucle infinie */
        while(1)
            pause();
    } else {        /* Parent process */
        printf("Parent process id : %d\n", getpid());

        /* Activation du signal SIGCHLD */
        unmask_signal(SIGCHLD);
        bind_signal(SIGCHLD,sig_handler);

        sleep(10);

        /* Envoi d'un signal SIGUSR1 au processus fils */
        int err = kill(pid,SIGUSR1);
        ERROR_IF(err == -1, "Erreur kill");

        /* Boucle infinie */
        while(1)
            pause();
    }

    return EXIT_SUCCESS;
}

static void mask_all_signals(void)
{
    sigset_t set;
    int err = sigfillset(&set);
    ERROR_IF(err == -1, "Erreur sigfillset");
    err = sigprocmask(SIG_SETMASK, &set, NULL);
    ERROR_IF(err == -1, "Erreur sigprocmask");
}

static void unmask_signal(int sigNum)
{
    /* Création d'un set de signal ne contenant que le signal sigNum */
    sigset_t set;
    int err = sigemptyset(&set);
    ERROR_IF(err == -1, "Erreur sigemptyset");
    err = sigaddset(&set, sigNum);
    ERROR_IF(err == -1, "Erreur sigaddset");
    /* Ajoute à la liste des signaux bloqués ceux présents dans set */
    err = sigprocmask(SIG_UNBLOCK, &set, NULL);
    ERROR_IF(err == -1, "Erreur sigprocmask");

    /* Algorithme alternatif : Ici on aurrait pu aussi récupérer
     * le set existant et démasquer le signal. */
    //sigset_t set;
    /* Récupération du set actif */
    //int err = sigprocmask(SIG_SETMASK, NULL, &set);
    //ERROR_IF(err == -1, "Erreur sigprocmask");
    /* Démasquage du signal sigNum */
    //err = sigdelset(&set, sigNum);
    //ERROR_IF(err == -1, "Erreur sigdelset");
    /* Application du nouveau set */
    //err = sigprocmask(SIG_SETMASK, &set, NULL);
    //ERROR_IF(err == -1, "Erreur sigprocmask");
}

static void bind_signal(int sigNum, void(*handler)(int))
{
    /* Définition des signaux bloqués pendant l'exécution du handler, en
     * supplément de ceux bloqués dans le processus et du signal sigNum */
    sigset_t set;
    int err = sigemptyset(&set);
    ERROR_IF(err == -1, "Erreur sigemptyset");

    /* Définition des paramètres de la gestion du signal */
    struct sigaction act;
	act.sa_handler = handler;   /* handler du signal sigNum */
    act.sa_mask = set;          /* signaux bloqués en supplément */
	act.sa_flags = 0;           /* flags par défaut */
	err = sigaction(sigNum, &act, NULL);
    ERROR_IF(err == -1, "Erreur sigaction");
}

static void sig_handler(int sigNum)
{
    switch(sigNum) {
    case SIGUSR1:
        printf("SIGUSR1 received (pid=%d) : stopping this process\n", getpid());
        exit(EXIT_SUCCESS);
        break;
    case SIGCHLD:
        printf("SIGCHLD received : my child has been killed\n");
        /* Libération des ressources associées au processus fils
         * pour qu'il ne devienne pas un zombie */
        int status;
        pid_t pid = wait(&status);
        ERROR_IF(pid == -1, "Erreur wait");
        printf("Process %d terminated, it will not turn into a zombie!\n", pid);
        break;
    default:
        printf("Unexpected signal received (%d), aborting!\n", sigNum);
        exit(EXIT_FAILURE);
    }
}
