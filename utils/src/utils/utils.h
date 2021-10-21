#ifndef _UTILS_
#define _UTILS_

/* General */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Commons */
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

/* Funciones */

/*
    @NAME:  crearID
    @DESC:  Retorna un id unico segun n
    @ARGS:  n - Es el puntero a una variable globlal
                usada para id de algo, esta variable
                debe estar inicializada en 0
 */
int crearID(int *n);

#endif
