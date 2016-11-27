#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

//retourne un entier lu dans le pipe passe en parametre
static int read_pipe(int pipe[]);

//ecrit la caleur dans le pipe passe en parametre
static void write_pipe(int pipe[], int toSend);

static long get_time_usec(void);




int main(){
	
	
/*----------------------------------------creation de 2 pipes ----------------------------*/
	//creation d'un pipe
	int p0[2];
	int p1[2];
	//0 : read | 1 : write
	pipe(p0);
	pipe(p1);
/*------------------------------------fin creation de 2 pipes ----------------------------*/
	
	int pid ;
	
	pid = fork() ; 
	
	if(pid == 0){
		//FILS
			
		int var ;
	
		printf("processus fils \n");
		//on va read sur p1[0]
		close(p1[1]);
		//on va write sur p0[1]
		close(p0[0]);
	
		int i ;
		for(i = 0 ; i < 100 ; i++ ) {
			var = read_pipe(p1);
			var ++ ;
			write_pipe(p0, var);
		}
		close(p1[0]);
		close(p0[1]);
		
	}else {
		//PERE
		int var = 0 ;
		long beginning = get_time_usec() ;
	
		close(p0[1]);
		close(p1[0]);
	
		int i ; 
		for(i = 0 ; i < 100 ; i++){
			write_pipe(p1,var);
			var = read_pipe(p0);
			var ++ ;
			//printf("%d \n",var);
		}
		long duration = get_time_usec() - beginning ;
		
		close(p0[0]);
		close(p1[1]);
	
		double rate = (2*100*sizeof(int)*8)/(double)(duration);
		printf("Rate %f\n", rate);
	}
	
	return 0 ; 
}

static int read_pipe(int pipe[]){
	int buffer ;
	read(pipe[0],&buffer,sizeof(int));
	return buffer ;	
}

static void write_pipe(int pipe[], int toSend){
	write(pipe[1], &toSend, sizeof(int));	
}

static long get_time_usec(void){
	struct timeval tv ;
	gettimeofday(&tv, NULL);
	return(tv.tv_sec*1000000L)+tv.tv_usec ;
}