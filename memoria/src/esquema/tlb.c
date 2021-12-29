#include "tlb.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <utils/utils.h>

#define FIFO_TLB alg_tlb
#define LRU_TLB  alg_tlb

t_list* tlb;
t_list * tlb_information;

int alg_tlb = 0;
int TLB_MISS_COUNT = 0;
int TLB_HIT_COUNT  = 0;

void iniciar_tlb(){

    tlb = list_create();
    tlb_information = list_create();

    for(int i=0; i<configuracion.CANTIDAD_ENTRADAS_TLB; i++){ 
        tlb_t* registro = malloc(sizeof(tlb_t));
        registro->marco = TLB_EMPTY;
        registro->pid = TLB_EMPTY;
        registro->pagina = TLB_EMPTY;
        registro->modificado = 0;
        list_add(tlb, (void*)registro);
    }
    loggear_mensaje("Tlb Inicializada");
}

int buscar_en_tlb(tab_pags* tabla, int pagina){

    if (configuracion.CANTIDAD_ENTRADAS_TLB == 0) return TLB_MISS;

    int pid = tabla->pid;

    for(int i=0; i<list_size(tlb); i++){

        tlb_t *reg = list_get(tlb, i);
        if(reg->pid != pid) continue;
        if(reg->pagina == pagina) return comportamiento_TLB_HIT(tabla->pid, reg);
    }

    return comportamiento_TLB_MISS(tabla->pid, pagina);
}

tlb_info_t* tlb_info_obtener(int pid)
{
    int i = 0;
    tlb_info_t* elegido = NULL;

    while (elegido == NULL) {
        tlb_info_t* elemento = list_get(tlb_information, i);
        if (elemento->pid == pid) return elemento;
        i++;
    }

    return elegido;
}

int comportamiento_TLB_HIT(int pid, tlb_t* reg){

    char mensaje[100];
    sprintf(mensaje, "TLB HIT - PID: %d - Pagina: %d - Marco: %d", pid, reg->pagina, reg->marco);
    loggear_mensaje(mensaje);

    tlb_info_t* info_tlb = tlb_info_obtener(pid);
    info_tlb->hits = info_tlb->hits + 1;

    TLB_HIT_COUNT++;
    usleep(configuracion.RETARDO_ACIERTO_TLB);
    return reg->marco;
}

int comportamiento_TLB_MISS(int pid, int pagina){

    char mensaje[100];
    sprintf(mensaje, "TLB MISS - PID: %d - Pagina: %d", pid, pagina);
    loggear_mensaje(mensaje);

    tlb_info_t* info_tlb = tlb_info_obtener(pid);
    info_tlb->miss = info_tlb->miss + 1;

    TLB_MISS_COUNT++;
    usleep(configuracion.RETARDO_FALLO_TLB);
    return TLB_MISS;
}

void actualizar_tlb(int pid, int marco, int pagina){

    if (configuracion.CANTIDAD_ENTRADAS_TLB == 0) return;

    int victima = tlb_obtener_victima();
    tlb_t* reg = list_get(tlb, victima);

    if(reg->modificado == 1) paginas_actualizar_modificado(reg->pid, reg->pagina);

    // log_info(logger_tlb, "Victima: PID %d | PAG %d | REG TLB %d | MARCO %d", reg->pid, reg->pagina, victima, reg->marco);


    reg->pid         =  pid;
    reg->marco       =  marco;
    reg->pagina      =  pagina;
    reg->alg_tlb     =  alg_comportamiento_tlb();
    reg->alg         =  alg_comportamiento();
    reg->modificado  =  0;
}

int tlb_obtener_victima(){
    
    int victima = tlb_lugar_libre();
    if(victima != -1) return victima;

    return algoritmo_tlb();
}

int tlb_lugar_libre(){

    int tamanio = list_size(tlb);

    for (int i=0; i < tamanio; i++)
    {
        tlb_t * reg = list_get(tlb, i);

        if (reg->pid == TLB_EMPTY) return i;
    }

    return -1;
}

void paginas_actualizar_modificado(int pid, int pagina){
    tab_pags* tabla = buscar_page_table(pid);
    pag_t* reg      = list_get(tabla->tabla_pag, pagina);
    reg->modificado = 1;
}

int fifo_tlb()
{
    int tamanio = list_size(tlb);
    int tlb_victima = LRU_C;
    int victima = -1;

    for(int i =0; i < tamanio; i++) {

       tlb_t * reg = list_get(tlb, i);

       if (reg->FIFO_TLB < tlb_victima) {
           victima = i;
           tlb_victima = reg->FIFO_TLB;
       }
    } 
    return victima;
} 

// Tiene el mismo comportamiento que fifo 
int lru_tlb()  
{
    return fifo_tlb();
}

void tlb_page_use_fifo(tlb_t* reg){}// what??

void tlb_page_use_lru(tlb_t* reg){
    reg->alg_tlb = suma_atomica(&FIFO_TLB);
}


tlb_t* buscar_reg_en_tlb(int pid, int n_pagina)
{
    int tamanio = list_size(tlb);

    for (int i=0; i < tamanio; i++) {
        tlb_t *reg = list_get(tlb, i);
        if (reg->pid == pid && reg->pagina == n_pagina) {
            comportamiento_TLB_HIT(pid, reg);
            return reg;
        }
    }

    comportamiento_TLB_MISS(pid, n_pagina);
    return NULL;
}

void eliminar_tlb_info()
{
    int tamanio = list_size(tlb_information);
    for (int i=0; i < tamanio; i++) {
        tlb_info_t *element = list_remove(tlb_information, 0);
        free(element);
    }
    list_destroy(tlb_information);
}

void tlb_info_agregar(int pid)
{
    tlb_info_t* element = malloc(sizeof(tlb_info_t));
    element->pid = pid;
    element->hits = 0;
    element->miss = 0;
    list_add(tlb_information, element);
}
