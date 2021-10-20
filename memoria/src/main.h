#ifndef _MAIN_MEMORIA_
#define _MAIN_MEMORIA_

#include "tests/tests.h"

/*
    @NAME: terminar_programa
    @DESC: Se encarga de liberar todas las estructuras y de hacer lo necesario para
           que el programa termine sin memory leaks y sin errores
 */
void terminar_programa();

void atender_cliente(void* arg);

#endif