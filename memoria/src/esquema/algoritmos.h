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
extern int (*alg_comportamiento)();
extern void (*tlb_page_use)(tlb_t* reg);
extern int (*alg_comportamiento_tlb)();

int fifo_tlb();
int lru_tlb();
int clock_buscar_puntero();
int clock_buscar_00(tab_pags* tabla);
int clock_buscar_01(tab_pags* tabla);

void page_use(int pid, int marco, pag_t* p, int n_pagina, int codigo);
 
t_victima clock_fijo(int pid, tab_pags* tabla);
t_victima clock_dinamico(int pid, tab_pags* tabla);
t_victima lru_dinamico(int pid, tab_pags* tabla);
t_victima lru_fijo(int pid, tab_pags* tabla);
t_victima lru_en_pag_table(tab_pags* tabla);

int alg_comportamiento_lru();
int alg_comportamiento_clock_modificado();
int alg_comportamiento_tlb_fifo();
int alg_comportamiento_tlb_lru();

void tlb_insert_page(int pid, int n_pagina, int marco, int codigo);
void page_use_clock_modificado(pag_t*);
void page_use_lru(pag_t*);

void tlb_page_use_fifo(tlb_t* reg);
void tlb_page_use_lru(tlb_t* reg);

tlb_t* tlb_obtener_registro(int pid, int pagina);
void tlb_actualizar_entrada_vieja(int pid, int pagina);

#endif
