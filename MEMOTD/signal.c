#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

void handler (int signum){
	switch (signum) {
		case SIGUSR2 : printf("Signal SIGUSR2 recu \n"); break ;
		case SIGUSR1 : printf("Signal SIGUSR1 recu \n"); break ;
					pthread_exit(NULL);
		default : break ; 
	}
}

void* mon_thread (void * arg){

	sigset_t sig ;
	sigfillset(&sig);
	sigdelset(&sig,SIGUSR1); //tous les signaux sont bloques sauf SIGUSR1
	
	pthread_sigmask(SIG_SETMASK, &sig, NULL);
	
	struct sigaction exit_action;
	exit_action.sa_handler = handler;
	exit_action.sa_mask = sig ;
	exit_action.sa_flags = 0 ;
	
	sigaction(SIGUSR1, &exit_action,NULL);
	printf("infini\n");
	while(1){
		sleep(1);
	}
	
	pthread_exit(NULL);
}

int main() {
	printf("pid = %d \n",getpid());
	
	pthread_t id ;
	pthread_create(&id, NULL,mon_thread,NULL);
	
	sigset_t sig;
	sigfillset(&sig); //par defaut tous les signaux sont bloques
	sigdelset(&sig, SIGUSR2); //sauf SIGUSR2
	
	pthread_sigmask(SIG_SETMASK,&sig,NULL);
	
	//definition signal handler pour SIGUSR2
	struct sigaction exit_action;
	exit_action.sa_handler = handler ; //handler utilise
	exit_action.sa_mask = sig; //signaux bloques pendant le handler
	exit_action.sa_flags = 0 ; //par defaut
	
	sigaction(SIGUSR2,&exit_action,NULL);
	
	sleep(1);
	pthread_kill(id,SIGUSR1);
	
	pthread_exit(NULL);
	return 0 ;
}