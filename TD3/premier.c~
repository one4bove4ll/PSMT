#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int res = 12345 ;

int * px = NULL ;
int * py = NULL ; 
pthread_mutex_t verroupx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t verroupy = PTHREAD_MUTEX_INITIALIZER;

void* mon_threadpx(void * arg){

  int max = *(int*)arg ;
  int i ;
  for(i = 0 ; i < max  ; i++ ){ 
    pthread_mutex_lock(&verroupx);
    printf("Voila la valeur contenue dans px  : %d\n",*px); 
    pthread_mutex_unlock(&verroupx);
  }

  pthread_exit(&res);

}

void* mon_threadpy(void * arg){

  int max = *(int*)arg ;
  int i ;
  for(i = 0 ; i < max  ; i++ ){ 
    pthread_mutex_lock(&verroupy);
    printf("Voila la valeur contenue dans py  : %d\n",*py); 
    pthread_mutex_unlock(&verroupy);
  }

  pthread_exit(&res);

}


int main() {

  int x = 1 ;
  px = &x ;

  int y = 0 ;
  py = &y ; 

  int m ;
  scanf("%d",&m);
 

  pthread_t id1 ;
  pthread_create(&id1,NULL,mon_threadpx,&m);
  pthread_t id2 ;
  pthread_create(&id2,NULL,mon_threadpy,&m);

  int j = 0 ; 

  while(j<m){
    if(pthread_mutex_trylock(&verroupx)==0){
      if(pthread_mutex_trylock(&verroupy)==0){
	px = NULL ;
	py = NULL ;    
	printf("Valeur de j : %d \n",j);
	px = &x ;
	py = &y ; 
	j++ ;	
	pthread_mutex_unlock(&verroupy);	
      }
      pthread_mutex_unlock(&verroupx);
    }
  }



  //pthread_cancel(id1);

  //void * pretour ;
  //pthread_join(id1,&pretour);
  //printf("Le retour est %d \n",*(int*)pretour);
  //printf("Le main attend le thread1 \n");
  pthread_exit(NULL);

  return 0 ;

}
