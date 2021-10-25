#ifndef _ALGORITMOS_MEMORIA_
#define _ALGORITMOS_MEMORIA_

#define BIT_USO algoritmo
#define LRU algoritmo

int LRU_C = 0;

typedef struct alg_t{
    int pid;    
    pag_t* pag;
}alg_t;

alg_t (*lru)(int pid);

alg_t lru_dinamico(int pid);
alg_t lru_fijo(int pid);
alg_t lru_en_pag_table(tab_pags* tabla);
#endif