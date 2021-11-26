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

extern int id_procesos;
extern int ids_memoria;

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

int min_get(int , int);

int max_get(int , int);

#endif
