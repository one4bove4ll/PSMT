#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<pthread.h>
#include<time.h>
#include<signal.h>

#define NB_MAGICIENS 4


// -----------------------------------
// -- VARIABLES PRIVÉES DU MAGICIEN --
// -----------------------------------
int _pid;
int _pv;
unsigned short _cote;
// -----------------------------------

// Définition du type opaque magicien,
// on le représente seuelement par son PID
// car les autres ne doivent pas connaître ses PV
// et son camp  !
typedef int Magicien;

// Structure pour la mémoire partagé
typedef struct Game {
	Magicien magiciens[NB_MAGICIENS]; //tableau des magiciens
	int ig_magiciens; // nombre de magiciens en jeu
	pthread_mutex_t lock; //mutex pour protéger les accès à la zone
} Game;

// ---------
// -- SHM --
// ---------
key_t key = (int) 1205;
int shm;
Game *game;
// --------

void creerXmagiciens();
int sommeDigitsPid(int pid);
void initialisation();
int rand_a_b(int a, int b);
void attendreAleatoirement();
void jeterSort();
void receptionSort(int sig);
void enterrement();
void magicien();

int main(void)
{
	//NOTE : Garry Poutter est le process père

	// Création de la SHM
	if((shm = shmget(key, sizeof(Game), 0666 | IPC_CREAT)) == -1)
	{
		perror("Erreur création de la SHM\n");
		exit(EXIT_FAILURE);
	}

	// Attachement de la zone mémoire
	if((game = shmat(shm,0,0))<0)
	{
		perror("Erreur attachement de la zone mémoire !\n");
		exit(EXIT_FAILURE);
	}

	//Initialisation de la structure jeu
	pthread_mutex_init(&game->lock, NULL); //init mutex
	game->ig_magiciens = 0; // pas de magiciens en jeu au début

	creerXmagiciens();

	printf("début du jeu, il y a %d magiciens \n\n\n", game->ig_magiciens);

	return 0;
}

/**
 * Crée les magiciens de la partie
 */
void creerXmagiciens()
{
	int pid=0, i;
	for(i=1;i<(NB_MAGICIENS);i++)
	{
		switch(pid=fork())
		{
			case -1: //erreur lors du fork
				perror("Erreur lors du fork() \n");
				exit(EXIT_FAILURE);
				break;

			case 0: //on est dans un processus fils
				magicien();
				exit(EXIT_SUCCESS);
				break;

			default: //on est chez Gary Poutter
				break;

		}
	}
	//Gary Poutter redevient un magicien normal
	magicien();
}

int sommeDigitsPid(int pid)
{
	if(pid == 0)
		return 0;
	else
		return (pid % 10) + sommeDigitsPid(pid/10);
}
/**
 * Initialise le magicien avant le jeu
 */
void initialisation()
{
	//pour les signaux
	struct sigaction action;

	srand(time(NULL)); //initialisation PRNG

	_pid = getpid();
	//si somme des digits du pid pair -> côté force (0) , sinon côté obscur(1)
	_cote = (sommeDigitsPid(_pid) % 2 ? 1 : 0);
	_pv = 10; // affectation des pv initiaux

	//prépare les masques de signaux (pour les sorts)
	sigset_t sig_mask;
	//on bloque tous les signaux
	if(sigfillset(&sig_mask) != 0)
	{
		perror("Erreur sigfillset \n");
		exit(EXIT_FAILURE);
	}

	//on autorise SIGUSR1
	if(sigdelset(&sig_mask, SIGUSR1) != 0)
	{
		perror("Erreur ajout SIGUSR1 \n");
		exit(EXIT_FAILURE);
	}

	//on autorise SIGUSR2
	if(sigdelset(&sig_mask, SIGUSR2) != 0)
	{
		perror("Erreur ajout SIGUSR2 \n");
		exit(EXIT_FAILURE);
	}

	//on applique le masque
	if(sigprocmask(SIG_SETMASK, &sig_mask, NULL) != 0)
	{
		perror("Erreur sigprocmask \n");
		exit(EXIT_FAILURE);
	}

	//on attache les handlers

	//pour SIGUSR1
	action.sa_handler = receptionSort;
	action.sa_flags=0;
	if(sigaction(SIGUSR1, &action, NULL) != 0)
	{
		perror("Erreur attachement handler SIGUSR1\n");
		exit(EXIT_FAILURE);
	}

	//pour SIGUSR2
	action.sa_handler = receptionSort;
	action.sa_flags = 0;
	if(sigaction(SIGUSR2, &action, NULL) != 0)
	{
		perror("Erreur attachement handler SIGUSR2 \n");
		exit(EXIT_FAILURE);
	}



	//donne connaissance aux autres de son existence
	//pour ce faire on utilise une mémoire partagée
	//dans laquelle se situe la structure Game

	pthread_mutex_lock(&game->lock); //entre en CS
	game->magiciens[game->ig_magiciens] = _pid;
	game->ig_magiciens++;
	pthread_mutex_unlock(&game->lock);//sort de CS

}

/**
 * Tire un nombre aléatoire dans [a,b[
 */
int rand_a_b(int a, int b)
{
	    return rand()%(b-a) +a;
}

void attendreAleatoirement()
{
	int delay = rand_a_b(1,5); // on borne le temps entre 1 et 5s
	sleep(delay);
}

void jeterSort()
{
	//Pour jeter un sort on va utiliser les signaux
	// SIGUSR1 : sort bon
	// SIGUSR2 : sort mauvais

	//tirage au sort de l'adversaire
	int opponent = rand_a_b(0,NB_MAGICIENS-1); //on borne par les indices du tableau

	//envoi des signaux
	if(_cote)
	{
		printf("[%d] je jettre un mauvais sort sur %d\n", _pid, game->magiciens[opponent]);
		kill(game->magiciens[opponent], SIGUSR2);
	}
	else
	{
		printf("[%d] je jette un bon sort sur %d\n", _pid, game->magiciens[opponent]);
		kill(game->magiciens[opponent], SIGUSR1);
	}
}

void receptionSort(int sig)
{
	//on traite la réception des différents signaux
	switch(sig)
	{
		case SIGUSR1:
			_pv++;
			printf("[%d] J'ai pris un bon sort : mes pv %d\n", _pid, _pv);
			break;

		case SIGUSR2:
			_pv-=2;
			printf("[%d] J'ai pris un mauvais sort : mes pv %d\n", _pid, _pv);
			break;

		default:
			fprintf(stderr, "Signal reçu inconnu !\n");
			exit(EXIT_FAILURE);
	}
}

/**
 * Lorsqu'un magicien est mort
 */
void enterrement()
{
	printf("[%d] Je suis mort !!\n", _pid);
	pthread_mutex_lock(&game->lock);
	game->ig_magiciens--;
	pthread_mutex_unlock(&game->lock);
}

/**
 * Définit le comportement d'un magicien
 */
void magicien()
{
	initialisation();
	printf("Je suis le magicien %d du côté %d\n", _pid, _cote);
	sleep(1); //on laisse le temps aux magiciens d'entrer en jeu ..

	while(_pv > 0)
	{
		attendreAleatoirement();
		jeterSort();
	}
	enterrement();
}
