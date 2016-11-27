#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "gestion_erreur.h"

typedef void *(*fct_ptr_t)(void *);

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void boucle(int *max)
{
    int err = pthread_mutex_lock(&mut);
    ERROR_IF(err != 0, "Erreur pthread_mutex_lock");

	printf("j'ai le mutex\n");
	for (int i = 0; i < *max; i++) {
		printf("tid=%lu\n", pthread_self());
		sleep(1);
	}

	err = pthread_mutex_unlock(&mut);
    ERROR_IF(err != 0, "Erreur pthread_mutex_unlock");
}

int main()
{
    pthread_t tid[2];
    int err, max = 6;

    for (int i = 0; i < 2; i++) {
        err = pthread_create(&tid[i], NULL, (fct_ptr_t)boucle, &max);
        ERROR_IF(err != 0, "Erreur pthread_create");

        for (int j = 0; j < (max / 2); j++) {
            printf("main thread=%lu\n", pthread_self());
            sleep(1);
        }

        /* On cancel les thread plus tôt, le mutex ne sera pas libéré */
        err = pthread_cancel(tid[i]);
        ERROR_IF(err != 0, "Erreur pthread_cancel");
        sleep(1);
    }

    return EXIT_SUCCESS;
}
