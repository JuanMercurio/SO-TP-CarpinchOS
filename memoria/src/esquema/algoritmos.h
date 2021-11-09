#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#define BIT_USO  algoritmo
#define LRU      algoritmo

#include "estructuras.h"
#include "paginacion.h"

extern int LRU_C;
extern int LRU_TLB;
extern int FIFO_TLB;

extern int (*algoritmo_tlb)();
extern t_victima (*algoritmo_mmu)(int, tab_pags*);

pag_t* clock_fijo(int pid, tab_pags* tabla);
pag_t* clock_dinamico(int pid, tab_pags* tabla);
pag_t* lru_dinamico(int pid, tab_pags* tabla);
pag_t* lru_fijo(int pid, tab_pags* tabla);
int fifo_tlb();
int lru_tlb();
pag_t* lru_en_pag_table(tab_pags* tabla);

#endif
