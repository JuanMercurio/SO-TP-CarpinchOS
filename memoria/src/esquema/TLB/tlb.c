#include "tlb.h"

int buscar_en_tlb(int pid, int pagina){

    /* TODO: Cantidad de TLB Hit totales
             Cantidad de TLB Hit por carpincho indicando carpincho y cantidad.
             Cantidad de TLB Miss totales.
             Cantidad de TLB Miss por carpincho indicando carpincho y cantidad.
    */

    for(int i=0; i<list_size(tlb); i++){

        t_tlb *reg = list_get(tlb, i);
        if(reg->pid==0 || reg->pid!=pid) continue;
        if(reg->pagina==pagina) return reg->marco;
    }

    return TLB_MISS;
}