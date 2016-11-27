#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

char * tab [] = {"prog1","prog2","prog3"};

int main () {
	
		int i ;
		for(i = 0 ; i < 3 ; i++) {
			switch(fork()){
				case -1 : printf("Erreur au fork \n");
				case 0 : execl(tab[i],tab[i],NULL);
			}
		}
	
		return 0 ;
}
