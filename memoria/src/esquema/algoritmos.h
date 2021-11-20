#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#define BIT_USO  algoritmo
#define LRU      algoritmo

#include "estructuras.h"
#include "paginacion.h"

typedef struct t_clock{
    tab_pags* tabla;
    int pagina;
    pag_t* reg;
}t_clock;

extern int LRU_C;
extern int LRU_TLB;
extern int FIFO_TLB;

extern int (*algoritmo_tlb)();
extern t_victima (*algoritmo_mmu)(int, tab_pags*);

int fifo_tlb();
int lru_tlb();
int clock_buscar_puntero();
int clock_buscar_00(tab_pags* tabla);
int clock_buscar_01(tab_pags* tabla);
 
t_victima clock_fijo(int pid, tab_pags* tabla);
t_victima clock_dinamico(int pid, tab_pags* tabla);
t_victima lru_dinamico(int pid, tab_pags* tabla);
t_victima lru_fijo(int pid, tab_pags* tabla);
t_victima lru_en_pag_table(tab_pags* tabla);

#endif
