#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER ;  

void* handler(){ //action a effectuer lors dun cancel ou lors du pop
	pthread_mutex_unlock(&lock);
	printf("Je libere le mutex \n");
}

void* mon_thread(void * arg){
	int max = *(int*)arg ;

	pthread_cleanup_push(handler,NULL); //debut de la zone de securite
	pthread_mutex_lock(&lock);
	printf("J'ai le mutex ! \n");
	
	int i ;
	for(i = 0 ; i < max ; i++){
		printf("Voila mon tid : %d \n", (int)pthread_self());
		sleep(1);
	}
	pthread_cleanup_pop(1); //fin de la zone de securite
//dans la zone de securite, si il y a un cancel, le programme va pop (donc executer de lehandler) avant de partir
//sinon il va jusquau pop et execute quand meme
}


int main() {

	int arg1 = 6;

	int j;
	for(j = 0 ; j<2 ; j++){
		pthread_t id1 ;
		pthread_create(&id1,NULL,mon_thread,&arg1);
		int i ;
		for(i = 0; i<arg1/2 ; i++){
			printf("Voila mon tid : %d (1)\n",(int)pthread_self());
			sleep(1);
		}
		pthread_cancel(id1);
		sleep(1);
	}	
 

	pthread_exit(NULL); 
	return 0 ;

}
