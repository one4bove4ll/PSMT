#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void handler() {
	pthread_mutex_unlock(&lock);
	printf("Le mutex est libere \n");
}

void* mon_thread(void * arg){
	
	pthread_cleanup_push(handler,NULL); //debut de la zone de securite
	pthread_mutex_lock(&lock);
	printf("Jai le mutex \n");
	
	//faire quelquechose
	sleep(1);
	
	pthread_cleanup_pop(1);//fin zone securite
	
	//dans la zone de securite, si il y a un cancel le programme va pop (execute le handler)
	//si il n'y a pas de cancel il va finir lexecution et pop quand meme
	
	pthread_exit(NULL);
}

int main () {
	pthread_t id1 ;
	
	pthread_create(&id1,NULL,mon_thread,NULL);
	sleep(1);
	pthread_cancel(id1);
	
	pthread_exit(NULL);
	return 0 ;
}