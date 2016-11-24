#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

sigset_t maskmain ;

void handlerUsr1 (int sig){
	printf("Signal 1 recu \n");
}

void handlerUsr2(int sig){
	pthread_exit(NULL);
}

void* mon_thread(void * arg){
	int max = *(int*)arg ;

	//mise a jour du masque pour le thread
	sigset maskthread;
	sigfillset(&maskthread);
	sigdelset(&maskthread,10);
	pthread_sig_mask(SIG_SETMASK,&maskthread,&maskmain);

	signal(SIGUSR1,handlerUsr1);	

}


int main() {
	sigifllset(&maskmain);
	sigdelset(&maskmain,12);

	int arg1 = 6;

	pthread_t id1 ;
	pthread_create(&id1,NULL,mon_thread,&arg1);
 
	signal(SIGUSR2,handlerUsr2);	

	pthread_exit(NULL); 
	return 0 ;

}
