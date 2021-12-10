#include "algoritmos.h"
#include "tlb.h"
#include "tlb.h"
#include "../signals/signal.h"

#include <stdlib.h>
#include <utils/utils.h>

int LRU_C = 0;
int LRU_TLB = 0;
int FIFO_TLB = 0;

t_clock p_clock;


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


			if(registro->presente == 0 || registro->algoritmo == -1) continue;
			if (registro->tlb == 1) {
                tlb_t* reg = tlb_obtener_registro(t->pid, j);
                if (reg == NULL) printf("perro estas en cualquiera\n");
                registro->algoritmo = reg->alg;
                registro->marco = reg->marco; 
                registro->modificado = reg->modificado;
                registro->presente = 1;
                registro->tlb = 1;
            }
            
            if (registro->algoritmo <= LRU_min) {
				LRU_min     = registro->algoritmo;
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
	victima.pid        = pid_victima;
	victima.pagina     = n_pagina;

	return victima;
}

t_victima lru_en_pag_table(tab_pags* tabla){

    int LRU_min = LRU_C, n_pagina;
    pag_t* pagina; 

    for (int i=0; i<list_size(tabla->tabla_pag); i++) {

        pag_t* registro = list_get(tabla->tabla_pag, i);
		if (registro->presente == 0 || registro->algoritmo == -1) continue;

        if (registro->tlb == 1) {
            tlb_t* reg = tlb_obtener_registro(tabla->pid, i);
            registro->algoritmo = reg->alg_tlb;
            registro->marco = reg->marco; 
            registro->modificado = reg->modificado;
            registro->presente = 1;
            registro->tlb = 1;
        }

        if (registro->algoritmo < LRU_min) {
            LRU_min  = registro->algoritmo;
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

tlb_t* tlb_obtener_registro(int pid, int pagina)
{
    int tamanio = list_size(tlb);

    for (int i=0; i < tamanio; i++) {

        tlb_t* reg = list_get(tlb, i);
        if (reg->pid == pid && reg->pagina == pagina) {
            return reg;
        }
    }

    return NULL;
}

t_victima lru_fijo(int pid, tab_pags* tabla){
    
    return lru_en_pag_table(tabla);
}

int clock_buscar_puntero()
{
    int cantidad_procesos = list_size(tablas.lista);

    for(int i=0; i < cantidad_procesos; i++)
    {
        tab_pags* tabla = list_get(tablas.lista, i);
        if(tabla->p_clock != -1) return i;
    }

    return -1;
}

int clock_buscar_00(tab_pags* tabla)
{
    int i;

    if(tabla->p_clock == -1) 
        i = 0;
    else
        i = tabla->p_clock;

    int iteracion = 0;
    int tamanio = list_size(tabla->tabla_pag);

    while(iteracion != tamanio)
    {
        if(i == list_size(tabla->tabla_pag)) i = 0;
        pag_t* reg = list_get(tabla->tabla_pag, i);
        if(reg->presente != 1 || reg->tlb == 1) 
		{
			if(i+1 == tamanio) 
				i = 0;
			else
            i++;
        
			iteracion++;
			continue;
		}

        if(reg->modificado == 0 && reg->algoritmo == 0) 
        {

            if(i+1 == tamanio)
                tabla->p_clock = 0;
            else
            tabla->p_clock = i+1;
            
            return i;
        }

       if(i+1 == tamanio) 
            i = 0;
        else
            i++;
        
        iteracion++;
    }

    return -1;
}

int clock_buscar_01(tab_pags* tabla)
{
    int i;

    if(tabla->p_clock == -1) 
        i = 0;
    else
        i = tabla->p_clock;

    int iteracion = 0;
    int tamanio = list_size(tabla->tabla_pag);

    while(iteracion != tamanio)
    {
        if(i == list_size(tabla->tabla_pag)) i = 0;
        
        pag_t* reg = list_get(tabla->tabla_pag, i);

        if(reg->presente != 1 || reg->tlb == 1) 
		{
			if(i+1 == tamanio) 
				i = 0;
			else
            i++;
        
			iteracion++;
			continue;
		}

        /* codigo horrible  */
        if(reg->modificado == 1 && reg->algoritmo == 0) 
        {
            if(i+1 == tamanio)
                tabla->p_clock = 0;
            else
                tabla->p_clock = i+1;
                
            return i;
        }
        else
            reg->algoritmo = 0;


        if(i+1 == tamanio) 
            i = 0;
        else
            i++;
        
        iteracion++;
    }

    return -1;
}

t_victima clock_fijo(int pid, tab_pags* tabla)
{
    if(tabla->p_clock == -1) tabla->p_clock = 0;
    
    int pagina = -1;
    while(pagina == -1)
    {
        printf("A buscar 00 \n");
        pagina = clock_buscar_00(tabla);
        if(pagina != -1) break;
        printf("A buscar 01 \n");
        pagina = clock_buscar_01(tabla);
    }

    pag_t* reg = list_get(tabla->tabla_pag, pagina);
    reg->presente = 0;

    t_victima victima;    
    victima.marco      = reg->marco;
    victima.modificado = reg->modificado;
    victima.pagina     = pagina;
    victima.pid        = pid;

    return victima;
}

t_victima clock_dinamico(int pid, tab_pags* tabla)
{
    int posicion = clock_buscar_puntero();
    int cantidad_procesos = list_size(tablas.lista);
    tab_pags* t; 
    int pagina = -1;
    while(pagina == -1)
    {
        t = list_get(tablas.lista, posicion);

        pagina = clock_buscar_00(t);
        if(pagina != -1) break;

        pagina = clock_buscar_01(t);
        if(pagina != -1) break;

        if(posicion+1 == cantidad_procesos) 
            posicion = 0;
        else
            posicion++;

        t->p_clock = -1;
    }

    pag_t* p = list_get(t->tabla_pag, pagina);

    t_victima victima;

    victima.marco      =  p->marco;
    victima.modificado =  p->modificado;
    victima.pagina     =  pagina;
    victima.pid        =  t->pid;

    return victima;
}


void page_use(int pid, int marco, pag_t* p, int n_pagina, int codigo)
{
    printf("Usando el pid: %d, pagina %d \n", pid, n_pagina);
    if (p->tlb == 1) {
        tlb_t *reg = buscar_reg_en_tlb(pid, n_pagina);
        tlb_page_use(reg);
        reg->alg = alg_comportamiento();
        if (codigo == WRITE) {
            reg->modificado = WRITE;
        }
        return;
    }

    tlb_insert_page(pid, n_pagina, marco, codigo);

    p->tlb        = 1;
    if(p->modificado == WRITE) p->modificado = WRITE;
    p->presente   = 1;
    p->algoritmo  = alg_comportamiento();
    p->marco      = marco;

}

int alg_comportamiento_lru()
{
    return suma_atomica(&LRU_C);
}

int alg_comportamiento_clock_modificado()
{
    return 1;
}

void tlb_insert_page(int pid, int n_pagina, int marco, int codigo)
{
    int victima = tlb_obtener_victima();
    
    tlb_t* reg = list_get(tlb, victima);
    if (reg->pid != -1) actualizar_victima_de_tlb(reg);

    reg->pid = pid;
    reg->pagina = n_pagina;
    reg->marco = marco;
    reg->modificado = codigo == WRITE ? 1 : 0;
    reg->alg_tlb = alg_comportamiento_tlb();
    reg->alg = alg_comportamiento();
}

int alg_comportamiento_tlb_fifo()
{
    return suma_atomica(&LRU_C);
}

int alg_comportamiento_tlb_lru()
{
    return suma_atomica(&LRU_C);
}
