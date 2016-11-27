
//rpasqua@laas.fr

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
	

void handlerPere(int sig){

	switch(sig){
		case 10 : printf("(pere) Signal recu 10\n"); exit(0); break ;
		
		case 17 : printf("(pere) J'ai tue le fils \n");  break ; 
		default : break ;
	}

}

void handlerFils(int sig){
	switch(sig){
		case 10 : 
			printf("(fils) Signal 10 \n");
			exit(0);
			break;
		default : break ;
	}
}

void proc_fils(){
	sigset_t maskmain;
	sigfillset(&maskmain);
	sigdelset(&maskmain,10);
	signal(SIGCHLD,handlerFils);
	printf("Je suis le fils, pid = %d \n",getpid());
	while(1){}
}

void proc_pere(int pid){
	sigset_t maskmain ;
	sigfillset(&maskmain);
	sigdelset(&maskmain,10);
	sigdelset(&maskmain,17);
	signal(17,handlerPere);
	signal(10,handlerPere);
	printf("Je suis le pere, voile mon pid : %d \n",getpid() );
	printf("Je suis le pere, voila le pid du fils : %d \n",pid);
	sleep(10);
	printf("(pere) : On envoie le signal 17 \n");
	kill(pid,10);
	kill(pid,17);
	while(1){}
}

int main() {

	int pid = 0 ;
	switch(pid = fork()){
		case -1 : printf("erreur \n") ; exit(-1) ; break ;
		case 0 : proc_fils() ; break ;
		default : proc_pere(pid) ; break ;
	}	


	return 0 ;
}
