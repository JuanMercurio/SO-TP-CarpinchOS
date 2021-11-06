#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#define BIT_USO  algoritmo
#define LRU      algoritmo

#include "paginacion.h"

extern int LRU_C;

extern pag_t* (*lru)(int, tab_pags*);

pag_t* lru_dinamico(int pid, tab_pags* tabla);
pag_t* lru_fijo(int pid, tab_pags* tabla);
pag_t* lru_en_pag_table(tab_pags* tabla);

#endif
