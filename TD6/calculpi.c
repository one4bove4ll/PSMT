#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>


int main() { 
 
  int i ;
  int randval ;

  for (i = 0 ; i < 10 ; i ++){    
    randval = rand();
    printf("%d \n",randval);
  }

  int j = RAND_MAX ;
  printf("Plus grand rand : %d \n",j);


  return 0 ;


}
