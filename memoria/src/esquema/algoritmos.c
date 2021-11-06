#include "algoritmos.h"

int LRU_C = 0;

int clock_mejorado();

pag_t* lru_dinamico(int pid, tab_pags* tabla)
{
    pag_t* pagina;
    int LRU_min = LRU_C;

    for(int i=0; i<list_size(tablas.lista); i++)
    {
        tab_pags* tabla = list_get(tablas.lista, i);
        for(int j=0; j<list_size(tabla->tabla_pag); j++)
        {
            pag_t* registro = list_get(tabla->tabla_pag, j);
            if(registro->LRU < LRU_min){
                LRU_min = registro->LRU;
                pagina = registro;
            }
        }
    }
    return pagina;
}

pag_t* lru_fijo(int pid, tab_pags* tabla){
    
    return lru_en_pag_table(tabla);
}

pag_t* lru_en_pag_table(tab_pags* tabla){

    int LRU_min = LRU_C;
    pag_t* pagina; 

    for(int i=0; i<list_size(tabla->tabla_pag); i++){

        pag_t* registro = list_get(tabla->tabla_pag, i);
        if(registro->LRU < LRU_min){
            LRU_min = registro->LRU;
            pagina = registro;
        }
    }
    return pagina;
}





