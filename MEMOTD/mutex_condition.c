#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER ;

void * monthread (void * arg){
	int n = *(int *) arg ;
	int condition_insatisfaite = 0 ;
	
	//attente des ressources
	printf("le thread (%d) attend \n",(int)pthread_self());
	pthread_mutex_lock(&verrou);
	while(condition_insatisfaite){
		pthread_cond_wait(&condition,&verrou); //attente d'un signal
		condition_insatisfaite = 1 ;
	}
	
	
	//debut du travail / zone critique
	printf("le thread (%d) travaille \n",(int)pthread_self());
	sleep(1);
	
	//on rend les ressources / fin zone critique
	
	pthread_cond_broadcast(&condition); // envoie un signal a tous les cond_wait
	pthread_mutex_unlock(&verrou);
	printf("le thread (%d) rend les ressources \n",(int)pthread_self());
	
	pthread_exit(NULL);
}


int main (){
	
	pthread_t id1 ;
	pthread_t id2 ;
	
	int arg1 =0 ;
	int arg2 = 0 ;
	
	pthread_create(&id1,NULL,monthread,&arg2);
	pthread_create(&id1,NULL,monthread,&arg1);
	
	pthread_exit(NULL);
	return 0 ;
}
