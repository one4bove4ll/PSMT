#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

float max = RAND_MAX ;
int d = RAND_MAX/2 ;
int nb_fleches = 0 ;
int nb_carre = 0 ;
int nb_cercle = 0 ; 
int nb_fleches_tot = 100000000;
int nb_threads = 1 ;
pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER ; 

void* monthread(void *arg){
  float x ;
  float y ;

  int sous_tot_carre = 0 ;
  int sous_tot_cercle = 0 ;
  
  int i = 0 ;
  for(i ; i < nb_fleches_tot/nb_threads ; i++ ){
    x = rand()-max/2 ;
    y = rand()-max/2 ;

    if(x*x+y*y<max*max/4){
      sous_tot_cercle ++ ;      
    }
    
    sous_tot_carre ++ ;
  }

  pthread_mutex_lock(&verrou);
  nb_carre += sous_tot_carre ;
  nb_cercle += sous_tot_cercle ;
  pthread_mutex_unlock(&verrou);

  pthread_exit(NULL);
}


int main(){
  clock_t begin = clock() ;

  printf("Nb de threads = %d \n",nb_threads);

  int argument = 1 ;
  pthread_t id1 ;
  pthread_t id2 ;
  pthread_t id3 ;
  pthread_t id4 ;
  
  pthread_create(&id1,NULL,monthread,&argument);
  //pthread_create(&id2,NULL,monthread,&argument);
  //pthread_create(&id3,NULL,monthread,&argument);
  //pthread_create(&id4,NULL,monthread,&argument);

  pthread_join(id1,NULL);
  //pthread_join(id2,NULL);
  //pthread_join(id3,NULL);
  //pthread_join(id4,NULL);
  
  float resultat = 4 * ((float)nb_cercle) /((float)nb_carre) ;
  printf("Nb_cercle = %d\n",nb_cercle);
  printf("Nb_carre = %d\n",nb_carre);
  printf("Le resultat est %f\n",resultat);

  clock_t end = clock() ; 
  double execution_time = (double)(end -begin) /CLOCKS_PER_SEC ;
  printf("Le temps d'execution est : %f\n",execution_time);
  pthread_exit(NULL);
  return 0 ; 
}
