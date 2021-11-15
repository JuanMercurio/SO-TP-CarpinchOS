#include "algoritmos.h"

#include <stdlib.h>

int LRU_C = 0;
int LRU_TLB = 0;
int FIFO_TLB = 0;

int clock_mejorado();

t_victima lru_dinamico(int pid, tab_pags* tabla)
{
    pag_t* pagina;
    int LRU_min = LRU_C, n_pagina, pid_victima;

    for(int i=0; i<list_size(tablas.lista); i++)
    {
        tab_pags* t = list_get(tablas.lista, i);
        for(int j=0; j<list_size(t->tabla_pag); j++)
        {
            pag_t* registro = list_get(t->tabla_pag, j);
            if(registro->LRU < LRU_min){
                LRU_min     = registro->LRU;
                pagina      = registro;
                n_pagina    = j;
                pid_victima = t->pid;
                
            }
        }
    }

    pagina->presente = 0;

    t_victima victima;    
    victima.marco      = pagina->marco;
    victima.modificado = pagina->modificado;
    victima.pid        = tabla->pid;
    victima.pagina     = n_pagina;

    return victima;
}

t_victima lru_fijo(int pid, tab_pags* tabla){
    
    return lru_en_pag_table(tabla);
}

t_victima lru_en_pag_table(tab_pags* tabla){

    int LRU_min = LRU_C, n_pagina;
    pag_t* pagina; 

    for(int i=0; i<list_size(tabla->tabla_pag); i++){

        pag_t* registro = list_get(tabla->tabla_pag, i);
        if(registro->LRU < LRU_min){
            LRU_min  = registro->LRU;
            pagina   = registro;
            n_pagina = i;
            
        }
    }
    
    pagina->presente = 0;

    t_victima victima;    
    victima.marco      = pagina->marco;
    victima.modificado = pagina->modificado;
    victima.pid        = tabla->pid;
    victima.pagina     = n_pagina;

    return victima;
}


int fifo_tlb()
{
    return 0;
} 

int lru_tlb()  
{
    return 0;
}

t_victima clock_fijo(int pid, tab_pags* tabla)
{
    t_victima victima = {0};
    return victima;
}


t_victima clock_dinamico(int pid, tab_pags* tabla)
{
    t_victima victima = {0};
    return victima;
}
