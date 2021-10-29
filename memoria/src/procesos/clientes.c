#include "clientes.h"
#include "operaciones.h"
#include "../esquema/paginacion.h"

#include <matelib/matelib.h>
#include <mensajes/mensajes.h>
#include <utils/utils.h>
#include <sys/socket.h>
#include <pthread.h>

void atender_proceso(void* arg){
    int cliente = *(int*)arg;
    free(arg);

    handshake(cliente, MEMORIA);
    ejecutar_proceso(cliente);
}

void ejecutar_proceso(int cliente) {

    int pid = PID_EMPTY;
    t_list* tabla;
    while(1) //TODO: feo while
    {
        int operacion = recibir_operacion(cliente);
        switch (operacion)
        {
        case NEW_INSTANCE:
            // comprobar si puede iniciar
            // si no puede envia pid como NOT_ASIGNED
            enviar_PID(&pid, cliente);
            tabla = iniciar_paginas(cliente, pid);
            break;
        
        case MEMALLOC:
            //ejecutar_malloc(tabla, cliente);
            break;

        case MEMFREE:
            break;

        case MEMREAD:
            break;

        case MEMWRITE:
            break;

        default:
            break;
        }
    }
}

t_list* iniciar_paginas(int cliente, int pid){
    // checkear si puede iniciar
    // si no puede ser iniciado: pthread_exit(0)
    tab_pags* tabla = malloc(sizeof(tab_pags));
    tabla->pid = pid;
    tabla->tabla_pag = list_create();

    add_new_page_table(tabla);
    // enviar_instancia();  ?

    return tabla->tabla_pag;
}

void enviar_PID(int *pid, int cliente){
    *pid = crearID(&ids_memoria);
    enviar_int(cliente, *pid);
}