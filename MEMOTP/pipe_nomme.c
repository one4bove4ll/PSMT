#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
Etape 1 mkfifo
Etape 2 int fdRead = open(...
Etape 2.5 int fdRead = open()
Etape 3 read_pipe | write_pipe
Etape 4 close
Etape 5 unlink

*/

/* Retourne un entier lu dans le decripteur passé en paramètre */
static int read_pipe(int fd);

/* Ecrit la valeur toSend dans le decripteur passé en paramètre */
static void write_pipe(int fd, int toSend);

/* Retourne le temps actuel en microsecondes */
static long get_time_usec(void);

/* Procédure effectuant l'écriture dans le pipe en premier */
static void first_writer(void);

/* Procédure effectuant la lecture dans le pipe en premier */
static void first_reader(void);

int main(){
	
	//creation des fifos 
	mkfifo("fifo1",S_IRUSR|S_IWUSR|S_IRGRP);
	mkfifo("fifo2",S_IRUSR|S_IWUSR|S_IRGRP);	
	
	
	int pid ;
	switch (pid = fork()){
		case 0 : first_reader(); break ;
		default : first_writer(); break ;
	}
	
	return 0 ;
}

static int read_pipe(int fd){
	int buffer;
	read(fd, &buffer, sizeof(int));
	return buffer ;
}

static void write_pipe(int fd, int toSend){
	write(fd,&toSend,sizeof(int));
}

static long get_time_usec(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000L) + tv.tv_usec;
}

static void first_writer(){
	sleep(1);
	//printf("Le pere \n");
	int token = 0 ;
	int fdWrite = open("fifo1", O_WRONLY); //creation des pipes
	int fdRead = open("fifo2", O_RDONLY);
	
	long beginning = get_time_usec() ;
	int i ;
	for(i=0 ; i<100;i++){
		//printf("pere ecrit %d \n", token);
		write_pipe(fdWrite,token);
		
		token = read_pipe(fdRead);
		//printf("pere lit %d \n", token);
		token ++ ;
	}
	long duration = get_time_usec() - beginning ;
	
	close(fdWrite);
	close(fdRead);
	
	double rate = (2*100*sizeof(int)*8)/(double)(duration);
	printf("Rate %f\n", rate);
}

static void first_reader(){
	//printf("le fils \n");
	int token ;
	int fdRead = open("fifo1",O_RDONLY);
	int fdWrite = open("fifo2",O_WRONLY);
	int i ;
	for(i = 0 ; i < 100 ; i++){
		
		token = read_pipe(fdRead);
		//printf("fils lit %d \n",token);
		token ++ ;
		//printf("fils ecrit %d \n",token);
		write_pipe(fdWrite,token);
	}
	
	close(fdWrite);
	close(fdRead);
	
	unlink("fifo1");
	unlink("fifo2");
	
}