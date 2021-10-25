#include "algoritmos.h"
#include "paginacion.h"


int clock_mejorado();


alg_t lru_dinamico(int pid)
{
    alg_t value;
    int LRU_min = LRU_C;

    for(int i=0; i<list_size(tablas.lista); i++)
    {
        tab_pags* tabla = list_get(tablas.lista, i);
        for(int j=0; j<list_size(tabla->tabla_pag); j++)
        {
            pag_t* registro = list_get(tabla->tabla_pag, j);
            if(registro->LRU < LRU_min){
                LRU_min = registro->LRU;
                value.pid = tabla->pid; 
                value.pag = registro;
            }
        }
    }
    return value;
}

alg_t lru_fijo(int pid){
    
    alg_t value;
    tab_pags* tabla = buscar_page_table(pid);
    return lru_en_pag_table(tabla);
        
}

alg_t lru_en_pag_table(tab_pags* tabla){

    int LRU_min = LRU_C;
    alg_t value;

    for(int i=0; i<list_size(tabla->tabla_pag); i++){

        pag_t* registro = list_get(tabla->tabla_pag, i);
        if(registro->LRU < LRU_min){
            LRU_min = registro->LRU;
            value.pid = tabla->pid; 
            value.pag = registro;
        }
    }
    return value;
}





