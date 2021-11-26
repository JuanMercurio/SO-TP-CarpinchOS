#include "clientes.h"

#include "../esquema/algoritmos.h"
#include "../esquema/paginacion.h"
#include "../esquema/tlb.h"
#include "operaciones.h"

#include <matelib/matelib.h>
#include <mensajes/mensajes.h>
#include <pthread.h>
#include <sys/socket.h>
#include <utils/utils.h>


void atender_proceso(void* arg){

    int cliente = *(int*)arg;
    free(arg);
    
    char mensaje[100];
    sprintf(mensaje, "Se conecto un cliente - Socket: %d", cliente);
    loggear_mensaje(mensaje);

    handshake(cliente, MEMORIA);
    ejecutar_proceso(cliente);

    close(cliente);
}

void ejecutar_proceso(int cliente) {

    int pid = PID_EMPTY;
    t_list* tabla;
    bool conectado = true;

    while(conectado)
    {
        int operacion = recibir_operacion(cliente);
        switch (operacion)
        {
        case NEW_INSTANCE:
            pid = iniciar_proceso(cliente);
            enviar_int(cliente, pid);
            tabla = iniciar_paginas(cliente, pid);
            printf("termine de iniciar a %d\n", pid);
            break;

        case MEMALLOC:
            ejecutar_malloc(tabla, cliente);
            /* RECIBE EDE KERNEL EL TAMAÑO A ALLOCAR
                DEVUELVE UN INT CON LA DL O UN -1 PARA DENEGAR ASIGNACION
             */
            break;

        case MEMFREE:
            /* RECIBE EDE KERNEL EL TAMAÑO A LIBERAR
                DEVUELVE UN INT CON LA DL O UN -5 PARA DENEGAR 
             */

            break;

        case MEMREAD:
            /* recibe una estructura con el int del ptr origen un void* que es un char* con el nombre de una io (segun ejemplo)
         Se usa para obtener información de la memoria. Con la dirección lógica que nos proveen y  la tabla de páginas hacemos la traducción a direcciones
          físicas y retornamos el contenido. 
        En caso de que se quiera leer una posición de memoria errónea, se deberá devolver el error MATE_READ_FAULT definido en el
         enum mate_errors de la matelib correspondiente con el valor -6.
*/
            break;

        case MEMWRITE:
            break;

        case NEW_INSTANCE_KERNEL:
            pid = recibir_operacion(cliente); // del otro lado se envia solamente el cod op y el pid
            tabla = iniciar_paginas(cliente, pid);
            break;

        case MATE_CLOSE: recibir_mensaje(cliente); /* DEBERA ELIMINAR LAS TABLAS DEL PROCESO CERRADO?? */
         close(cliente);

        default:
            conectado = false;
            break;
        }
    }

    pthread_mutex_lock(&tablas.mutex);
    tablas_eliminar_proceso(pid);
    tlb_eliminar_proceso(pid);
    pthread_mutex_unlock(&tablas.mutex);

    //avisar a swamp

    char mensaje[100];
    sprintf(mensaje, "Se desconecto un cliente - Socket: %d - PID: %d", cliente, pid);
    loggear_mensaje(mensaje);
}

t_list* iniciar_paginas(int cliente, int pid)
{
    tab_pags* tabla = malloc(sizeof(tab_pags));

    tabla->pid        = pid;
    tabla->TLB_HITS   = 0;
    tabla->TLB_MISSES = 0;
    tabla->tabla_pag  = list_create();

    add_new_page_table(tabla);

    return tabla->tabla_pag;
}

void enviar_PID(int *pid, int cliente){ 
    *pid = crearID(&ids_memoria);
    enviar_int(cliente, *pid);
}



int iniciar_proceso(int proceso)
{
    int swamp = crear_conexion("127.0.0.1", "5003");
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

    int tamanio_buffer = recibir_int(cliente);
    if(tamanio_buffer != sizeof(int)*4) return -1;

    int size = 0;
    int* pid_cliente = recibir_buffer_t(&size, cliente);

    if(*pid == NOT_ASIGNED) 
    {
        *pid = *pid_cliente;
        iniciar_paginas(cliente, *pid);
    } 
    if(*pid != *pid_cliente)
    {
        //comportamiento de pid erroneo
        printf("Me llego un pid erroreo en memalloc\n");
    }
    else 
    {
        printf("El cliente esta en el sistema continuo con malloc\n");
        // memalloc();
    }

    return -1;
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



void tablas_eliminar_proceso(int pid){
    printf("eliminando Proceso %d\n", pid);
    int tamanio = list_size(tablas.lista);
    for(int i=0; i < tamanio; i++)
    {
        tab_pags* tabla = list_get(tablas.lista, i);
        if(tabla->pid == pid) 
        { 
           eliminar_proceso_i(i);
           return;
        }
    }
    
    abort();
}

void eliminar_proceso_i(int i){
    tab_pags* tabla = list_remove(tablas.lista, i);

    for(int i=0; i < list_size(tabla->tabla_pag); i++)
    {
        pag_t* reg = list_get(tabla->tabla_pag, i);
        if(reg->presente == 1)
        {
            int* marco = list_get(marcos, reg->marco);
            *marco = 0;
        }
    }

    free(tabla->tabla_pag);
}

void tlb_eliminar_proceso(int pid){

    int tamanio = list_size(tlb);

    for(int i=0; i < tamanio; i++)
    {
        tlb_t* reg = list_get(tlb, i);
        if(reg->pid == pid) reg->pid = TLB_EMPTY;
    }

}