#ifndef _COMPORTAMIENTO_CLIENTES_
#define _COMPORTAMIENTO_CLIENTES_

#include "../esquema/paginacion.h"


void mate_close_comportamiento(tab_pags *tabla, int cliente, bool *conectado);
void memalloc_comportamiento(tab_pags *tabla, int socket);
void memfree_comportamiento(tab_pags *tabla, int socket);
void memread_comportamiento(tab_pags *tabla, int socket);
void memwrite_comportamiento(tab_pags *tabla, int socket);
void new_instance_comportamiento(tab_pags *tabla, int socket, bool *controlador);
void new_instance_kernel_comportamiento(tab_pags* tabla, int cliente, bool *conectado);


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
void iniciar_paginas(tab_pags *tabla);

void enviar_PID(int *pid, int cliente);

tab_pags* tabla_init();

void comprobar_inicio(int estado, int socket);

int iniciar_proceso(int proceso);
void inicio_comprobar(tab_pags* tabla, int cliente, bool* conectado);

int swap_solicitud_iniciar(int);
void cliente_terminar(tab_pags* tabla, int cliente);

/*
    @NAME:  pid_valido
    @DESC:  dado un pid dice si existe en el sistema 
 */
bool pid_valido(int pid);

void tlb_eliminar_proceso(int pid);

void eliminar_proceso_i(int i);

void tablas_eliminar_proceso(int pid);

void clock_puntero_actualizar(int pid);

void tablas_imprimir_saturno();
#endif
