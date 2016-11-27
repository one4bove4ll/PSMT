#ifndef GESTION_ERREUR_H
#define GESTION_ERREUR_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/*
 * Macro utilisée pour afficher le message d'erreur msg passé en paramètre
 * si la condition cond est validée, puis arrêter le programme.
 * Le message affiché contient des informations supplémentaires concernant
 * le fichier de provenance, la fonction et le numéro de ligne concerné.
 * Si errno est défini, l'erreur correspondante est affichée.
 */
#define ERROR_IF(cond,msg) \
    if (cond) { \
        int errsv = errno; \
        fprintf(stderr, "%s -> (%s:%d) %s", \
                        __FILE__, __func__, __LINE__, msg); \
        if (errsv != 0) { \
            fprintf(stderr, ": "); \
            errno = errsv; \
            perror(NULL); \
        } else { \
            fprintf(stderr, "\n"); \
        } \
        exit(EXIT_FAILURE); \
    }


#endif /*GESTION_ERREUR_H*/
