#include "tlb.h"
#include "../configuracion/config.h"

#include <stdlib.h>
#include <stdio.h>

void iniciar_tlb(){

    tlb = list_create();

    for(int i=0; i<configuracion.CANTIDAD_ENTRADAS_TLB; i++){ 
        tlb_t* registro = malloc(sizeof(tlb_t));
        registro->marco = EMPTY;
        registro->pid = EMPTY;
        registro->pagina = EMPTY;
        list_add(tlb, (void*)registro);
    }
}

int buscar_en_tlb(int pid, int pagina){

    /* TODO: Cantidad de TLB Hit totales
             Cantidad de TLB Hit por carpincho indicando carpincho y cantidad.
             Cantidad de TLB Miss totales.
             Cantidad de TLB Miss por carpincho indicando carpincho y cantidad.
    */

    for(int i=0; i<list_size(tlb); i++){

        tlb_t *reg = list_get(tlb, i);
        if(reg->pid== EMPTY || reg->pid!=pid) continue;
        if(reg->pagina==pagina) return reg->marco;
    }

    return TLB_MISS;
}