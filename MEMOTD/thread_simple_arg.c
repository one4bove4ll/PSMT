# include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void* mon_thread(void *arg){
	
	int n = *(int*)arg;
	
	
	
	pthread_exit(NULL);
	
	//int res = 0 ;
	//pthread_exit(&res);
}

int main() {
	int n = 0 ;
	pthread_t id1 ;
	
	pthread_create(&id1,NULL,mon_thread,&n);
	pthread_join(id1,NULL);	
	
	//void *  pretour ;
	//pthread_join(id1,pretour);
	
	pthread_exit(NULL);
	
	return 0 ;
	
}