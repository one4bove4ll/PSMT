#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int * px = NULL ; //variable globale a proteger, doit etre un pointeur
pthread_mutex_t verroupx = PTHREAD_MUTEX_INITIALIZER ; //verrou/mutex sur cette variable


void * monthread (void * arg){
	int n = *(int * )arg ;
	
	
	pthread_mutex_lock(&verroupx); //debut zone critique
	printf("utilisation de la variable\n");
	pthread_mutex_unlock(&verroupx); //fin zone critique
	
	pthread_exit(NULL);
}

int main(){
	
	pthread_t id1;
	int arg = 0 ;
	
	pthread_create(&id1,NULL,monthread,&arg);
	
	
	if(pthread_mutex_trylock(&verroupx)==0){ //prendre le mutex si il est disponible uniquement
		printf("On a verouille le mutex sans blocage \n");
		pthread_mutex_unlock(&verroupx);
	}
	
	pthread_exit(NULL);
	
	return 0 ;
}