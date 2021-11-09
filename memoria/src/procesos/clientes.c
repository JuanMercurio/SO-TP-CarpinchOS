#include "clientes.h"

#include "../esquema/paginacion.h"
#include "../esquema/algoritmos.h"
#include "operaciones.h"

#include <matelib/matelib.h>
#include <mensajes/mensajes.h>
#include <pthread.h>
#include <sys/socket.h>
#include <utils/utils.h>


void atender_proceso(void* arg){
    int cliente = *(int*)arg;
    free(arg);

    handshake(cliente, MEMORIA);
    ejecutar_proceso(cliente);
}

void ejecutar_proceso(int cliente) {

    int pid = PID_EMPTY;
    t_list* tabla;
    bool conectado = true;
    while(conectado) //TODO: feo while
    {
        int operacion = recibir_operacion(cliente);
        switch (operacion)
        {
        case NEW_INSTANCE:
            pid = iniciar_proceso(cliente);
            enviar_int(cliente, pid);
            tabla = iniciar_paginas(cliente, pid);
            break;
        
        case MEMALLOC:
            comportamiento_memalloc(&pid, cliente);
            break;

        case MEMFREE:
            break;

        case MEMREAD:
            break;

        case MEMWRITE:
            break;

        default:
            conectado = false;
            break;
        }
    }
}

t_list* iniciar_paginas(int cliente, int pid){
    tab_pags* tabla = malloc(sizeof(tab_pags));
    tabla->pid = pid;
    tabla->tabla_pag = list_create();

    add_new_page_table(tabla);
    return tabla->tabla_pag;
}

void enviar_PID(int *pid, int cliente){ 
    *pid = crearID(&ids_memoria);
    enviar_int(cliente, *pid);
}



int iniciar_proceso(int proceso){

    int swamp = crear_conexion("127.0.0.1", "5003");
    printf("envio una solicitud a swamp");
    enviar_int(swamp, SOLICITUD_INICIO);

    int estado = recibir_int(swamp);
    comprobar_inicio(estado, proceso);

    int pid = crearID(&ids_memoria);

    enviar_int(swamp, pid);

    return pid;
}

void comprobar_inicio(int estado, int socket){
    if(estado == -1) 
    {
        enviar_int(socket, NOT_ASIGNED);
        pthread_exit(0);
    }
}

int comportamiento_memalloc(int* pid, int cliente){
    int pid_cliente = recibir_int(cliente);
    if(*pid == NOT_ASIGNED) *pid = pid_cliente;
    if(*pid != pid_cliente)
    {
        //comportamiento de pid erroneo
    }
    else 
    {
        memalloc();
    }
}


bool pid_valido(int pid){

    bool existe = false;
    pthread_mutex_lock(&tablas.mutex);

    for(int i=0; i<list_size(tablas.lista); i++){
        tab_pags* paginas = list_get(tablas.lista, i);
        existe = paginas->pid == pid;
        if(existe) break;
        
    }

    pthread_mutex_lock(&tablas.mutex);
    return existe;    
}