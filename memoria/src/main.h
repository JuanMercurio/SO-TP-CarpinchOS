#ifndef _MAIN_MEMORIA_
#define _MAIN_MEMORIA_

#include "tests/tests.h"

/*
    @NAME: terminar_programa
    @DESC: Se encarga de liberar todas las estructuras y de hacer lo necesario para
           que el programa termine sin memory leaks y sin errores
 */
void terminar_programa();

/*
    @NAME: administrar_clientes
    @DESC: Funciones que administran los hilos que se conectan a la memoria
 */
void administrar_clientes();

/*
    @NAME: atender_cliente
    @DESC: Hilo de cada cliente conectado
 */
void atender_cliente(void* arg);

#endif