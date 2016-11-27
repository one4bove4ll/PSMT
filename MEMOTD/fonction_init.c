#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_once_t fonction_init_once = PTHREAD_ONCE_INIT ; //variable once

void fonction_init(void){ //il faut separer la fonction a faire une fois
	printf("Le thread id=%d fait l'initialisation \n",(int)pthread_self());
}

void* mon_thread(void * arg) {
	
	printf("Hello je suis le thread %d \n",(int)pthread_self());
	pthread_once(&fonction_init_once, fonction_init); //il faut appeler la fonction a faire une fois de cette maniere
	sleep(1);
	
	pthread_exit(NULL);
}

int main () {
	pthread_t id1;
	pthread_t id2;
	
	pthread_create(&id1,NULL,mon_thread,NULL);
	pthread_create(&id2,NULL,mon_thread,NULL);
	
	pthread_exit(NULL);
	return 0 ;
}