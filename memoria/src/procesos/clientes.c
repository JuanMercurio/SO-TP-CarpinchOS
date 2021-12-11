#include "clientes.h"

#include "../esquema/algoritmos.h"
#include "../esquema/tlb.h"
#include "operaciones.h"

#include <matelib.h>

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

    handshake(cliente, "MEMORIA");
    ejecutar_proceso(cliente);

}

void ejecutar_proceso(int cliente)
{
    tab_pags* tabla = tabla_init();
    bool conectado = true;

    while(conectado)
    {
        int operacion = recibir_operacion(cliente);

        pthread_mutex_lock(&ram.mutex);
        switch (operacion)
        {
            case NEW_INSTANCE:
                new_instance_comportamiento(tabla, cliente, &conectado);
                break;

            case MEMALLOC:
                memalloc_comportamiento(tabla, cliente);
                break;

            case MEMFREE:;
                memfree_comportamiento(tabla, cliente);
                break;

            case MEMREAD:
                memread_comportamiento(tabla, cliente);
                break;

            case MEMWRITE:;
                memwrite_comportamiento(tabla, cliente);
                break;

            case NEW_INSTANCE_KERNEL:printf("llego desde KERNEL\n");
                new_instance_kernel_comportamiento(tabla, cliente, &conectado);
                
                break;

            case MATE_CLOSE:
                mate_close_comportamiento(tabla, cliente, &conectado);
                break;

            case SWAMP:
                printf("se conecto swamp\n");
                swap = cliente;
                enviar_mensaje(cliente, configuracion.TIPO_ASIGNACION);
                conectado = false;
                break;

            default:
                conectado = false;
                break;
        }
        pthread_mutex_unlock(&ram.mutex);
    }
sleep(40);
}

/* Functions */ 

void new_instance_comportamiento(tab_pags* tabla, int cliente, bool *conectado)
{   
    tabla->pid = crearID(&ids_memoria);
    int respuesta = swap_solicitud_iniciar(tabla->pid);

    if(respuesta == -1) 
    {
        enviar_int(cliente, -1);
        enviar_mensaje(cliente, "No es posible iniciar");
        free(tabla->tabla_pag);
        free(tabla);
        *conectado = false;
        return;
    }

    enviar_int(cliente, tabla->pid);
    // enviar_int(swap, tabla->pid);
    enviar_mensaje(cliente, "OK");
    iniciar_paginas(tabla);
}


void new_instance_kernel_comportamiento(tab_pags* tabla, int cliente, bool *conectado){ 

    tabla->pid = recibir_operacion(cliente); 
    int respuesta = swap_solicitud_iniciar(tabla->pid);

    if(respuesta == -1) 
    {
        enviar_int(cliente, -1);
        //enviar_mensaje(cliente, "No es posible iniciar");
        free(tabla->tabla_pag);
        free(tabla);
        *conectado = false;
        return;
    }
    enviar_int(cliente, 0);
    iniciar_paginas(tabla);
}


int swap_solicitud_iniciar(int pid){
    //return 0;
    printf("------------------------ENVIANDO A SWAMP Solicitud %d\n", pid);
    enviar_int(swap, INICIO);
    printf("Enviar un pid %d\n", pid);
    enviar_int(swap, pid);
    printf("------------------------ESPERANDO....\n");
    int estado = recibir_int(swap);
    printf("------------------------RECIBIDO DE SWAMP %d\n", estado);
    return estado;
}

void memalloc_comportamiento(tab_pags* tabla, int cliente)
{
    int tamanio = recibir_int(cliente);
    int dl = memalloc(tabla, tamanio);

    enviar_int(cliente, dl);
}


void memfree_comportamiento(tab_pags* tabla, int cliente)
{
    int dl = recibir_int(cliente);
    int result_memfree = memfree(tabla, dl);

    enviar_int(cliente, result_memfree);
}

void memread_comportamiento(tab_pags* tabla, int cliente)
{
    int dl = recibir_int(cliente);
    int tamanio = recibir_int(cliente);

    void* buffer = memread(tabla, dl, tamanio);
    if (buffer == NULL) {
        enviar_int(cliente, -1);
    }
    enviar_int(cliente, 0);
    enviar_int(cliente, tamanio);
    enviar_buffer(cliente, buffer, tamanio);
}

void memwrite_comportamiento(tab_pags* tabla, int cliente)
{
    int *size = malloc(sizeof(int));
    void* buffer = recibir_buffer_t(size, cliente);
    int dl = recibir_int(cliente);
    printf("MEMWRITE: recibio %s\n", (char*)buffer);
    int result_memwrite = memwrite(tabla, dl, buffer, *size);
    enviar_int(cliente, result_memwrite);
    free(size);

}

void mate_close_comportamiento(tab_pags *tabla, int cliente, bool *conectado)
{
    enviar_int(swap, BORRAR_CARPINCHO);
    enviar_int(swap, tabla->pid);
    int pudo = recibir_int(swap);
    printf("Resultado swap: %d\n", pudo);
    if (pudo == -1) abort();

    cliente_terminar(tabla, cliente);
    *conectado = false;
}


void iniciar_paginas(tab_pags* tabla)
{
    tabla->TLB_HITS   = 0;
    tabla->TLB_MISSES = 0;
    tabla->tabla_pag  = list_create();

    add_new_page_table(tabla);
}

void inicio_comprobar(tab_pags* tabla, int cliente, bool* conectado){
    
    int respuesta = swap_solicitud_iniciar(tabla->pid);

    if(respuesta == -1) 
    {
        enviar_mensaje(cliente, "No se pudo iniciar"); 
        free(tabla->tabla_pag);
        free(tabla);
        *conectado = false;
        return;
    }

    enviar_int(swap, tabla->pid);
    enviar_mensaje(cliente, "OK");
    iniciar_paginas(tabla);
}

void cliente_terminar(tab_pags* tabla, int cliente)
{ 
    tablas_eliminar_proceso(tabla->pid);
    tlb_eliminar_proceso(tabla->pid);

    //avisar a swamp

    char mensaje[100];
    sprintf(mensaje, "Se desconecto un cliente - Socket: %d - PID: %d", cliente, tabla->pid);
    loggear_mensaje(mensaje);
    close(cliente);
}

tab_pags* tabla_init()
{ 
    tab_pags *tabla= malloc(sizeof(tab_pags));
    tabla->pid = NOT_ASIGNED;
    return tabla;
}





void enviar_PID(int *pid, int cliente){ 
    *pid = crearID(&ids_memoria);
    enviar_int(cliente, *pid);
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
            printf("El coso a eliminar es: %d", reg->marco);
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
