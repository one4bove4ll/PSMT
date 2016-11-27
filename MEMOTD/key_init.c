#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

pthread_once_t fonction_init_once = PTHREAD_ONCE_INIT ;
pthread_key_t key ; // on cree une cle

void fonction_init() {
	pthread_key_create(&key,free); //ne doit etre fait qu'une seule fois !!
}

void * mon_thread(void * arg){

	pthread_once(&fonction_init_once, fonction_init);

	int *buf = malloc(sizeof(int));
	*buf = pthread_self() % INT_MAX;

	pthread_setspecific(key,buf); //on lie la cle a la variable
	//debut

	int* x = (int*)pthread_getspecific(key); // on recupere la variable en utilisant la cle
	*x = *x ++;
	//fin
	pthread_exit(NULL);
}

int main() {
	
	pthread_t id1 ;
	pthread_create(&id1,NULL,mon_thread,NULL);
	
	pthread_join(id1,NULL);
	pthread_exit(NULL);
	return 0 ;
}