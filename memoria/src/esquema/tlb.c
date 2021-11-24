#include "tlb.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define FIFO_TLB alg_tlb
#define LRU_TLB  alg_tlb

t_list* tlb;

int alg_tlb = 0;
int TLB_MISS_COUNT = 0;
int TLB_HIT_COUNT  = 0;

void iniciar_tlb(){

    tlb = list_create();

    for(int i=0; i<configuracion.CANTIDAD_ENTRADAS_TLB; i++){ 
        tlb_t* registro = malloc(sizeof(tlb_t));
        registro->marco = TLB_EMPTY;
        registro->pid = TLB_EMPTY;
        registro->pagina = TLB_EMPTY;
        registro->modificado = 0;
        list_add(tlb, (void*)registro);
    }
}

int buscar_en_tlb(tab_pags* tabla, int pagina){

    int pid = tabla->pid;

    for(int i=0; i<list_size(tlb); i++){

        tlb_t *reg = list_get(tlb, i);
        if(reg->pid != pid) continue;
        if(reg->pagina == pagina) return comportamiento_TLB_HIT(tabla, reg);
    }

    return comportamiento_TLB_MISS(tabla, pagina);
}

int comportamiento_TLB_HIT(tab_pags* tabla, tlb_t* reg){

    char mensaje[100];
    sprintf(mensaje, "TLB HIT - PID: %d - Pagina: %d - Marco: %d", tabla->pid, reg->pagina, reg->marco);
    loggear_mensaje(mensaje);

    tabla->TLB_HITS++;
    TLB_HIT_COUNT++;
    usleep(configuracion.RETARDO_ACIERTO_TLB);
    return reg->marco;
}

int comportamiento_TLB_MISS(tab_pags* tabla, int pagina){

    char mensaje[100];
    sprintf(mensaje, "TLB MISS - PID: %d - Pagina: %d", tabla->pid, pagina);
    loggear_mensaje(mensaje);

    tabla->TLB_MISSES++;
    TLB_MISS_COUNT++;
    usleep(configuracion.RETARDO_FALLO_TLB);
    return TLB_MISS;
}

void actualizar_tlb(int pid, int marco, int pagina){

    int victima = tlb_obtener_victima();
    tlb_t* reg = list_get(tlb, victima);

    if(reg->modificado == 1) paginas_actualizar_modificado(reg->pid, reg->pagina);

    reg->pid         =  pid;
    reg->marco       =  marco;
    reg->pagina      =  pagina;
    reg->alg_tlb     =  suma_atomica(&alg_tlb);
    reg->modificado  =  0;
}

int tlb_obtener_victima(){
    
    int victima = tlb_lugar_libre();
    if(victima != -1) return victima;

    victima = algoritmo_tlb();

    return victima; 
}

int tlb_lugar_libre(){
    return 0;
}

void paginas_actualizar_modificado(int pid, int pagina){
    tab_pags* tabla = buscar_page_table(pid);
    pag_t* reg      = list_get(tabla->tabla_pag, pagina);
    reg->modificado = 1;
}

int fifo_tlb()
{
    int tamanio = list_size(tlb);
    int tlb_victima = FIFO_TLB;
    int victima;

    for(int i =0; i < tamanio; i++) 
    {
       tlb_t * reg = list_get(tlb, i);
       if(reg->FIFO_TLB < tlb_victima) 
       {
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

void tlb_page_use_fifo(tlb_t* reg){}

void tlb_page_use_lru(tlb_t* reg){
    reg->alg_tlb = suma_atomica(&LRU_TLB);
}
