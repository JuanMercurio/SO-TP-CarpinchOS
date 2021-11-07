#ifndef _MAIN_SWAMP_
#define _MAIN_SWAMP_

#include "tests/tests.h"
#include "configuracion/config.h"
#include <utils/utils.h> 
#include <conexiones/conexiones.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int server;

/*
    @NAME: terminar_programa
    @DESC: Se encarga de liberar todas las estructuras y de hacer lo necesario para
           que el programa termine sin memory leaks y sin errores
 */
void terminar_programa();
void crearArchivos();
void iniciar_swamp();
void atender_clientes();
int elegirMejorArchivo();

#endif
