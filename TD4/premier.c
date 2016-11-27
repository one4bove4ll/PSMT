#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int cpt = 4 ;
pthread_mutex_t lockcpt = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER ;

void* mon_thread(void * arg){

  int j = *(int*)arg ;

  printf("Je suis le thread %d \n",(int)pthread_self());

  //on attend les ressources
  pthread_mutex_lock(&lockcpt);
  while(cpt<j){
    pthread_cond_wait(&condition,&lockcpt);
  }
  printf("Le thread %d a obtenu les ressources nécessaires (%d). \n",(int)pthread_self(),j);
  cpt -= j ; 
  pthread_mutex_unlock(&lockcpt);
  
  //après avoir obtenu les ressources on travaille
  printf("Le thread %d commence a travailler.\n",(int)pthread_self()); //travail
  sleep(4);

  //on rend les ressources
  pthread_mutex_lock(&lockcpt);
  printf("Le thread %d rend les ressources obtenues (%d). \n",(int)pthread_self(),j);
  cpt += j ;
  pthread_cond_broadcast(&condition);
  pthread_mutex_unlock(&lockcpt);
  
  //Fin du thread
  printf("Fin du thread %d (%d) \n",(int)pthread_self(),j);
  pthread_exit(NULL);

}



int main() {

	int arg	1 = 1;
	pthread_t id1 ;
	pthread_create(&id1,NULL,mon_thread,&arg1);
 
	pthread_exit(NULL);

	return 0 ;

}
