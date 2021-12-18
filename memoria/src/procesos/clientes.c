#include "clientes.h"

#include "../esquema/algoritmos.h"
#include "../esquema/tlb.h"
#include "operaciones.h"
#include "../main.h"
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
    printf("Termino un carpincho\n");
}

void ejecutar_proceso(int cliente)
{
    tab_pags* tabla = NULL;
    tabla = tabla_init();
    bool conectado = true;
    int operacion;

    while(conectado)
    {
        operacion = recibir_operacion(cliente);

        pthread_mutex_lock(&ram.mutex);
        switch (operacion)
        {
            case NEW_INSTANCE:
                new_instance_comportamiento(tabla, cliente, &conectado);
                break;

            case MEMALLOC:
                memalloc_comportamiento(tabla, cliente);
                tablas_loggear_saturno();
                break;

            case MEMFREE:;
                memfree_comportamiento(tabla, cliente);
                tablas_loggear_saturno();
                break;

            case MEMREAD:
                memread_comportamiento(tabla, cliente);
                tablas_loggear_saturno();
                break;

            case MEMWRITE:;
                memwrite_comportamiento(tabla, cliente);
                tablas_loggear_saturno();
                break;

            case NEW_INSTANCE_KERNEL:
                new_instance_kernel_comportamiento(tabla, cliente, &conectado);
                
                break;

            case MATE_CLOSE:
                mate_close_comportamiento(tabla, cliente, &conectado);
                break;

            case SWAMP:
                log_info(logger_memoria, "Se conecto SWAMP.");
                swap = cliente;
                enviar_mensaje(cliente, configuracion.TIPO_ASIGNACION);
                conectado = false;
                break;
                
            case SUSPENCION: 
                log_info(logger_memoria, "-- SUSPENCION | PID %d -- ", tabla->pid);
                for(int i = 0 ; i < list_size(tabla->tabla_pag); i++ ){

                    pag_t  *pagina= buscar_en_tabPags(tabla, i);

                    if(pagina->marco == -1){
                        log_info(logger_memoria, "La pagina %d no esta en memoria.", i);
                        continue;
                    }

                    if (pagina->tlb == 1) {
                        log_info(logger_memoria, "Accedo a la pagina %d mediante TLB.", i);
                        tlb_t* reg = tlb_obtener_registro(tabla->pid, i);

                        pagina->algoritmo = reg->alg;
                        pagina->marco = reg->marco; 
                        pagina->modificado = reg->modificado;
                        log_info(logger_memoria, "Actualizo bit de modificado.");
                    }

                    if(pagina->modificado == 1) { 
                        enviar_pagina_a_swap(tabla->pid, i, pagina->marco);
                        log_info(logger_memoria, "La pagina %d fue modificada. La envio a SWAP.");
                        int aux = recibir_int(swap);;
                    }else log_info(logger_memoria, "La pagina %d no fue modificada. No la envio a SWAP.");
                    
                    log_info(logger_memoria, "Saco del marco %d a la pagina %d (si esta).",pagina->marco, i);
	                pagina_liberar_marco(pagina);
                    
                    if(pagina->tlb){
                        log_info(logger_memoria, "Saco de la TLB marco %d a la pagina %d", pagina->marco,i);
                        pagina_liberar_tlb(tabla->pid, i);
                    }    
                        
                    
                }
                log_info(logger_memoria, "-- FIN SUSPENCION --\n");
                break;

           /*  case VUELTA_A_READY: printf("recibi vuelta a ready de carpincho %d\n", cliente); //cambiar por comportamieto real
            // vuelve a tener sus paginas disponibles para seguir su ejecucion          
                break;*/
                
            default:
                conectado = false;
                break;
        }
        pthread_mutex_unlock(&ram.mutex);
    }
    if(cliente == swap){
        free(tabla);
        sem_init(&finalizar_conexion_swap, 0, 0);
        sem_wait(&finalizar_conexion_swap);//falta inicilizar seamforo
        sem_destroy(&finalizar_conexion_swap);
    } else free(tabla);
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
    enviar_mensaje(cliente, "OK");
    tlb_info_agregar(tabla->pid);
    iniciar_paginas(tabla);
    log_info(logger_memoria, "Inicio un proceso - PID: %d - Socket %d", tabla->pid, cliente);
}


void new_instance_kernel_comportamiento(tab_pags* tabla, int cliente, bool *conectado){ 

    tabla->pid = recibir_operacion(cliente); 
    int respuesta = swap_solicitud_iniciar(tabla->pid);

    if(respuesta == -1) 
    {
        enviar_int(cliente, -1);
        free(tabla->tabla_pag);
        free(tabla);
        *conectado = false;
        return;
    }
    enviar_int(cliente, 0);
    tlb_info_agregar(tabla->pid);
    iniciar_paginas(tabla);
    log_info(logger_memoria, "Inicio un proceso - PID: %d - Socket %d", tabla->pid, cliente);
}


int swap_solicitud_iniciar(int pid){
    log_info(logger_memoria, "------------------------ENVIANDO A SWAMP SOLICITUD INICIO %d");
    enviar_int(swap, INICIO);
    enviar_int(swap, pid);
    int estado = recibir_int(swap);
    log_info(logger_memoria, "------------------------RECIBIDO DE SWAMP %d\n", estado);
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

    free(buffer);
}

void memwrite_comportamiento(tab_pags* tabla, int cliente)
{
    int *size = malloc(sizeof(int));
    void* buffer = recibir_buffer_t(size, cliente);
    int dl = recibir_int(cliente);
    int result_memwrite = memwrite(tabla, dl, buffer, *size);
    enviar_int(cliente, result_memwrite);
    free(size);
    free(buffer);
}

void mate_close_comportamiento(tab_pags *tabla, int cliente, bool *conectado)
{
    enviar_int(swap, BORRAR_CARPINCHO);
    enviar_int(swap, tabla->pid);
    int pudo = recibir_int(swap);
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
    tablas_imprimir_saturno();
    if (tabla->p_clock != -1) clock_puntero_actualizar(tabla->pid);
    tablas_eliminar_proceso(tabla->pid);
    tlb_eliminar_proceso(tabla->pid);

    //avisar a swamp

    char mensaje[100];
    sprintf(mensaje, "Se desconecto un cliente - Socket: %d - PID: %d", cliente, tabla->pid);
    loggear_mensaje(mensaje);
    close(cliente);
}

void clock_puntero_actualizar(int pid)
{
    if (strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "CLOCK-M") != 0) return;

    int ganador;
    int procesos = list_size(tablas.lista);
    for (int i=0; i < procesos; i++) {

        tab_pags* t = list_get(tablas.lista, i);
        if (t->pid == pid) {
            ganador = procesos-1 == i ? 0 : i+1;
            break;
        }
    }

    if (procesos == 1) primero = true;

    tab_pags* t = list_get(tablas.lista, ganador);
    t->p_clock = 0;
}

tab_pags* tabla_init()
{ 
    tab_pags *tabla = malloc(sizeof(tab_pags));
    tabla->pid = NOT_ASIGNED;
    tabla->TLB_HITS = 0;
    tabla->TLB_MISSES = 0;
    tabla->tabla_pag = NULL;

    if (strcmp(configuracion.TIPO_ASIGNACION, "FIJA") == 0) {
        tabla->p_clock = 0;
    } else {
        tabla->p_clock = NOT_ASIGNED;
    }

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

    int tamanio = list_size(tabla->tabla_pag);
    for(int i = tamanio -1 ; i >= 0; i--)
    {
        pag_t* reg = list_remove(tabla->tabla_pag, i);
        if(reg->presente == 1)
        {
            marco_t* marco = list_get(marcos, reg->marco);
            marco->pid = 0;
        }
        free(reg);
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

