#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>


int semaphore1;
int semaphore2 ;

void wait_sem(int semaphore){ //P(s)
	struct sembuf buffer ;
	buffer.sem_num = 0;
	buffer.sem_flg = 0;
	buffer.sem_op = -1 ;
	semop(semaphore,&buffer,1);
}

void release_sem(int semaphore){ //V(s)
	struct sembuf buffer ;
	buffer.sem_num = 0 ;
	buffer.sem_flg = 0 ;
	buffer.sem_op = 1 ;
	semop(semaphore,&buffer,1);
}


void proc_fils(int* shared_memory) {
	printf("Je suis le fils (pid = %d)\n",getpid());
	int actual_value = 0 ;
	
	while(actual_value <20){
		//section critique
		//gestion synchronisation avec semaphore
		printf("(son) on attend s1 \n");
		wait_sem(semaphore1);
		printf("(son) on obtient s1 \n");
		//lecture SHM
		printf("(son) shared memory is : %d \n",*shared_memory);
		//ecriture SHM
		*shared_memory = *shared_memory +1 ;
		actual_value = *shared_memory ;
		printf("(son) %d written \n", *shared_memory);
		//gestion synchronisation
		release_sem(semaphore2);
		printf("(son) on rend s2 \n");
	}
}

void proc_pere(int pid,int* shared_memory) {
	printf("Je suis le pere (pid = %d)\n",getpid());
	printf("pid du fils = %d \n",pid);
	int actual_value = 0 ;
	
	wait_sem(semaphore2);
	
	while(actual_value<20){
		//section critique
		
		//ecriture SHM
		*shared_memory = *shared_memory +1 ;
		actual_value = *shared_memory ;
		printf("(father) %d written \n", *shared_memory);
		//gestion synchronisation
		release_sem(semaphore1);
		printf("(father) on rend s1 on attend s2 \n");
		wait_sem(semaphore2);
		printf("(father) on obtient s2 \n");
		//lecture SHM
		printf("(father) shared memory is : %d \n",*shared_memory);
		
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

	//creation d'un semaphore
	int key = 14111995 ;
	union semun arg ;
	arg.val = 1 ;
	semaphore1 = semget((key_t)key,1,0666|IPC_CREAT);
	//initialisation semaphore
	semctl(semaphore1,0,SETVAL,arg);
	//fin creation/initialisation
	
	/*----------------------------------creation d'un semaphore----------------------------------------------*/
	int key2 = 14111996 ;
	union semun arg2 ;
	arg2.val = 1 ;
	semaphore2 = semget((key_t)key,1,0666|IPC_CREAT);
	//initialisation semaphore
	semctl(semaphore2,0,SETVAL,arg);
	/*----------------------------------fin creation/initialisation------------------------------------------------*/
	
	
	
	/*-----------------------------creation zone de mémoire partagée-------------------------------------*/
	int key_shm = 123;
	int shared_memory_id ;
	int* shared_memory ;
	shared_memory_id = shmget((key_t)key_shm,sizeof(int),0666|IPC_CREAT);
	shared_memory = shmat(shared_memory_id,NULL,0);
	*shared_memory = 0 ;
	/*----------------------------- fin de la creation zone de mémoire partagée-----------------------*/
	
	
	
	switch(pid = fork()){
		case -1 : printf("erreur \n") ; exit(-1); break ;
		case 0 : proc_fils(shared_memory); break ;
		default : proc_pere(pid,shared_memory); break ;	
	}

	//destruction du semaphore
	semctl(semaphore1,0,IPC_RMID,0);
	
	semctl(semaphore2,0,IPC_RMID,0);
	
	
	
	return 0 ;
}
