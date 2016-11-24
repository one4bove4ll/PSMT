#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>

void wait_sem(int semaphore){
	struct sembuf buffer ;
	buffer.sem_num = 0;
	buffer.sem_flg = 0;
	buffer.sem_op = -1 ;
	semop(semaphore,&buffer,1);
}

void release_sem(int semaphore){
	struct sembuf buffer ;
	buffer.sem_num = 0 ;
	buffer.sem_flg = 0 ;
	buffer.sem_op = 1 ;
	semop(semaphore,&buffer,1);
}


void proc_fils(int semaphore) {
	printf("Je suis le fils (pid = %d)\n",getpid());	
	while(1){
		//section critique
		//attente du semaphore
		printf("(fils) j'attends le semaphore \n");
		wait_sem(semaphore);
		//saisie chaine
		printf("(fils) Veuillez rentrer une chaine \n");
		char m[5] ;
  		fgets(m,5,stdin);
		printf("(fils) Je lis %s \n",m);
		//liberation semaphore
		printf("(fils) je relache le semaphore \n");
		release_sem(semaphore);
	}
}

void proc_pere(int pid,int semaphore) {
	printf("Je suis le pere (pid = %d)\n",getpid());
	printf("pid du fils = %d ",pid);
	
	while(1){
		//section critique
		//attente du semaphore
		printf("(pere) j'attends le semaphore \n");
		wait_sem(semaphore);
		//saisie chaine
		printf("(pere) Veuillez rentrer une chaine \n");
		char m[5] ;
  		fgets(m,5,stdin);
		printf("(pere) Je lis %s \n",m);
		//liberation semaphore
		printf("(pere) je relache le semaphore \n");
		release_sem(semaphore);
	}
			
  
}

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array ;
	struct seminfo *__buff;
};

int main() {
	int pid = 0 ;

	//initialisation d'un semaphore
	int key = 14111995 ;
	int semaphore ;
	union semun arg ;
	arg.val = 1 ;
	semaphore = semget((key_t)key,1,0666|IPC_CREAT);
	semctl(semaphore,0,SETVAL,arg);
	//fin d'initialisation

	switch(pid = fork()){
		case -1 : printf("erreur \n") ; exit(-1); break ;
		case 0 : proc_fils(semaphore); break ;
		default : proc_pere(pid,semaphore); break ;	
	}

	//destruction du semaphore
	senctl(semaphore,0,IPC_RMID,0);
	
	return 0 ;
}
