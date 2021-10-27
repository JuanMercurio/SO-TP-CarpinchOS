#ifndef _ALGORITMOS_MEMORIA_
#define _ALGORITMOS_MEMORIA_

#define BIT_USO algoritmo
#define LRU algoritmo

#include "paginacion.h"

int LRU_C;

pag_t* (*lru)(int pid);

pag_t* lru_dinamico(int pid);
pag_t* lru_fijo(int pid, tab_pags* tabla);
pag_t* lru_en_pag_table(tab_pags* tabla);
#endif