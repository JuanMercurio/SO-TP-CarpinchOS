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

/*
    @NAME:  comportamiento_memalloc
    @DESC:  retorna la direccion logica del alloc o -1 si hay error
 */
int comportamiento_memalloc(int* pid, int cliente);

/*
    @NAME:  pid_valido
    @DESC:  dado un pid dice si existe en el sistema 
 */
bool pid_valido(int pid);

void tlb_eliminar_proceso(int pid);

void eliminar_proceso_i(int i);

void tablas_eliminar_proceso(int pid);

#endif
