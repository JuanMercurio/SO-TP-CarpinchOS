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

/* Variables Globales */

int id_procesos;

/* Funciones */

/*
    @NAME:  crearID
    @DESC:  Retorna un id unico segun n
    @ARGS:  n - Es el puntero a una variable globlal
                usada para id de algo
 */
int crearID(int *n);

/*
    @NAME:  suma_atomica
    @DESC:  Retorna el valor previa de n 
    @ARGS:  n - Es el puntero a una variable globlal
 */
int suma_atomica(int *n);

#endif
