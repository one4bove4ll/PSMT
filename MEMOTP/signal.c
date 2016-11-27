#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handlerPere(int sig){
	
	switch(sig){
		case 10 : printf("Signal 10 reçu \n"); break ; //SIGUSR1
		case 17 : printf("Signal 17 reçu : le fils est mort \n"); break ; //SIGCHLD
		default : break ;
	}
	
}

void handlerFils(int sig){
	
	switch(sig){
		case 10 : printf("Signal 10 reçu \n"); exit(0) ; break ; //SIGUSR1
		default : break ;
	}
	
}

void proc_fils() {
		
	sigset_t mask_fils ;
	sigfillset(&mask_fils); //tous est bloques
	sigdelset(&mask_fils,10); //sauf SIGUSR1 (10)
	signal(10,handlerFils); //on associe SIGCHLD(17) au handler
	
	while(1){}
	
}

void proc_pere(int pid) {
	//proc pere, le pid du fils est en argument
	printf("Je suis le pere voila mon pid %d \n",getpid());
	printf("Voila le pid du fils %d \n",getpid());
	sigset_t mask_pere ; //creation du mask on laisse passer SIGUSR1 et SIGCHLD
	sigfillset(&mask_pere);
	sigdelset(&mask_pere,10);
	sigdelset(&mask_pere,17);
	signal(17,handlerPere); //on associe aux handlers
	signal(10,handlerPere);
	
	sleep(1);
	kill(pid,10);
	
	while(1){}
	
}

int main() {
	
	int pid = 0 ;
	switch(pid = fork()){
		case -1 : printf("erreur fork \n") ; exit(-1); break ;
		case 0 : proc_fils() ; break ;
		default : proc_pere(pid) ; break ;
	}
	
	return 0 ; 
	
}