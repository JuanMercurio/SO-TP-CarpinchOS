#ifndef _ESQUEMA_MEMORIA_
#define _ESQUEMA_MEMORIA_

#include <commons/collections/list.h>
#include <utils/utils.h>

#include "../configuracion/config.h"
#include "algoritmos.h"
#include "estructuras.h"

#define TLB_MISS  -1
#define TLB_EMPTY -1 

/* TLB */
extern t_list * tlb;

extern int TLB_MISS_COUNT;
extern int TLB_HIT_COUNT;

extern int alg_tlb;

/*
    @NAME:  iniciar_tlb
    @DESC:  inicia la tlb poniendo todos sus registros en EMPTY
 */
void iniciar_tlb();

/*
    @NAME:  buscar_en_tlb
    @DESC:  busca en la tlb el frame en el cual esta
            la pagina del proceso especificado.
            Si no encuentra la pagina retorna TLB_MISS  
 */
int buscar_en_tlb(tab_pags* tabla, int pagina);

/* 
    @NAME:  actualizar_tlb
 */
void actualizar_tlb(int pid, int marco, int pagina);

int comportamiento_TLB_HIT(tab_pags* tabla, tlb_t* pagina);

int comportamiento_TLB_MISS(tab_pags* tabla, int pagina);

void paginas_actualizar_modificado(int pid, int pagina);

int tlb_obtener_victima();

int tlb_lugar_libre();

tlb_t* buscar_reg_en_tlb(int pid, int n_pagina);

#endif
