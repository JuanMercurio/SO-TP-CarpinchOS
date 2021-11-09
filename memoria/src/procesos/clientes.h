#ifndef _COMPORTAMIENTO_CLIENTES_
#define _COMPORTAMIENTO_CLIENTES_

#include "../esquema/paginacion.h"

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

void enviar_PID(int *pid, int cliente);

void comprobar_inicio(int estado, int socket);

int iniciar_proceso(int proceso);

#endif
