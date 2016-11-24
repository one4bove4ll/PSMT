#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int res = 12345 ;

void* mon_thread(void * arg){

  
  int loop = *(int*)arg ;
  int i ;
  for(i = 0 ; i < loop ; i++ ){
    printf("et mon courroux \n");
    sleep(1);
  }

  pthread_exit(&res);

}

int main() {

  pthread_t id1 ;
  int m ;
  scanf("%d",&m);
  int n ; 
  scanf("%d",&n);

  pthread_create(&id1,NULL,mon_thread,&n);

  int j;

  for (j= 0 ; j<m ; j++){
    printf("coucou \n");
    sleep(1);
  }

  //pthread_cancel(id1);

  //void * pretour ;
  //pthread_join(id1,&pretour);
  //printf("Le retour est %d \n",*(int*)pretour);
  //printf("Le main attend le thread1 \n");
  pthread_exit(NULL);

  return 0 ;

}
