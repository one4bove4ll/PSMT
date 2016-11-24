//rpasqua@laas.fr

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int p[2];
int p2[2];


void calcul_debit(struct timeval t_d, struct timeval t_f){
	int resultat;
	float duree_s = t_f.tv_sec - t_d.tv_sec ;
	float duree_us = t_f.tv_usec - t_d.tv_usec ;
	float duree_tot = duree_s + duree_us/1000000 ;

	if((1000 * 2 * 4 * 8)* (1E-6)/(duree_tot)<255){
		resultat = (1000 * 2 * 4 * 8)*(1E-6)/(duree_tot);
	}else {
		resultat = (1000 * 2 * 4 * 8)* 0.000001/(duree_tot)/8;
	}
	
	printf("Temps exec : %f \n",duree_tot);
	printf("Le resultat est : %d \n", resultat);
	//return resultat ;
}

void proc_fils(){
	struct timeval t_debut, t_fin ;
	int nb_a_ecrire = 0 ;
	int nb_lu ;
	printf("Je suis le fils, pid = %d \n",getpid());
	
	gettimeofday(&t_debut,NULL);
	while(nb_lu < 100){
		//lecture
		read(p2[0],&nb_lu,sizeof(int));
		printf("(fils) J'ai lu %d \n",nb_lu);
		//ecriture
		nb_a_ecrire = nb_lu +1 ;
		write(p[1],&nb_a_ecrire,sizeof(int));
		printf("(fils) J'ecris %d \n",nb_a_ecrire);
	}
	gettimeofday(&t_fin,NULL);
	//printf("Debit du fils : \n");
	calcul_debit(t_debut,t_fin);
}


void proc_pere(int pid){
	struct timeval t_debut, t_fin ;
	int nb_a_ecrire = 0 ;
	int nb_lu;
	//sleep(3);	
	printf("Je suis le pere, voila mon pid : %d \n",getpid() );
	printf("Je suis le pere, voila le pid du fils : %d \n",pid);
	
	//gettimeofday(&t_debut,NULL);
	
	while(nb_a_ecrire<100){
		//ecriture
		write(p2[1],&nb_a_ecrire, sizeof(int));
		printf("(pere) J'ecris %d \n",nb_a_ecrire);		
		//lecture
		
		read(p[0],&nb_lu, sizeof(int));
		printf("(pere)Je lis %d \n",nb_lu);	
		
		nb_a_ecrire = nb_lu +1; 
	}
	//gettimeofday(&t_fin,NULL);
	//printf("Débit du pere : \n");
	//calcul_debit(t_debut,t_fin);
}

int main() {

//creation des tubes 


	int pid = 0 ;
	pipe(p);
	pipe(p2);
	switch(pid = fork()){
		case -1 : printf("erreur \n") ; exit(-1) ; break ;
		case 0 : proc_fils() ; break ;
		default : proc_pere(pid) ; break ;
	}	


	return 0 ;
}
