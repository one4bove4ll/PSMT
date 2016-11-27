#ifndef GESTION_ERREUR_H
#define GESTION_ERREUR_H

#include <stdlib.h>
#include <stdio.h>

/*
 * Macro utilisée pour afficher le message d'erreur msg passé en paramètre
 * si la condition cond est validée, puis arrêter le programme.
 * Le message affiché contient des informations supplémentaires concernant
 * le fichier de provenance, la fonction et le numéro de ligne concerné.
 */
#define ERROR_IF(cond,msg) \
    if (cond) { \
        fprintf(stderr, "%s -> (%s:%d) %s\n", \
                        __FILE__, __func__, __LINE__, msg); \
        exit(EXIT_FAILURE); \
    }


#endif /*GESTION_ERREUR_H*/
