#include "tlb.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


t_list* tlb;

int TLB_MISS_COUNT = 0;
int TLB_HIT_COUNT  = 0;

void iniciar_tlb(){

    tlb = list_create();

    for(int i=0; i<configuracion.CANTIDAD_ENTRADAS_TLB; i++){ 
        tlb_t* registro = malloc(sizeof(tlb_t));
        registro->marco = TLB_EMPTY;
        registro->pid = TLB_EMPTY;
        registro->pagina = TLB_EMPTY;
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

}
