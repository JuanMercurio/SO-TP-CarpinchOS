#ifndef _COMPORTAMIENTO_CLIENTES_
#define _COMPORTAMIENTO_CLIENTES_

#include "../esquema/paginacion.h"

#define PID_EMPTY 0 

/*
    @NAME:  atender_proceso
    @DESC:  hilo que atiene a los procesos que se conectan
            a memoria
 */
void atender_proceso(void* arg);

/*
    @NAME:  ejecutar_proceso
    @DESC:  ejecuta los mensajes que le llegan al hilo de los 
            carpinchos
 */
void ejecutar_proceso(int cliente);

/*
    @NAME:  iniciar_paginas
    @DESC:  inicia y retorna la tabla de paginas de un proceso
 */
t_list* iniciar_paginas(int cliente, int pid);

/*
    @NAME:  idesginar_PID
    @DESC:  Designa el pid del proceso
 */
int desginar_PID(int cliente);

/*
    @NAME:  recibir_PID
    @DESC:  Recibe el pid que le manda un carpincho o el kernel
 */
int recibir_PID(int cliente);

#endif