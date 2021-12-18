#include "algoritmos.h"
#include "tlb.h"
#include "tlb.h"
#include "../signals/signal.h"

#include <stdlib.h>
#include <utils/utils.h>

int LRU_C = 0;
int LRU_TLB = 0;
int FIFO_TLB = 0;

int clock_puntero = 0;
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
               // registro->presente = 1;
               // registro->tlb = 1;
            }
            
            if (registro->algoritmo <= LRU_min) {
				LRU_min     = registro->algoritmo;
				pagina      = registro;
				n_pagina    = j;
				pid_victima = t->pid;

			}
		}
	}
	t_victima victima;    
	victima.marco      = pagina->marco;
	victima.modificado = pagina->modificado;
	victima.pid        = pid_victima;
	victima.pagina     = n_pagina;
    victima.tlb        = pagina->tlb;

	pagina->presente = 0;
    pagina->modificado = 0; //esto es para que cuando busque en swap la pagina este en modo read hasta que le diga que no
    pagina->tlb = 0;
	return victima;
}

t_victima lru_en_pag_table(tab_pags* tabla){

    int LRU_min = LRU_C, n_pagina;
    pag_t* pagina = NULL; 

    for (int i=0; i<list_size(tabla->tabla_pag); i++) {

        pag_t* registro = list_get(tabla->tabla_pag, i);
		if (registro->presente == 0 || registro->algoritmo == -1) continue;

        printf("hay para analizar\n");
        if (registro->tlb == 1) {
            tlb_t* reg = tlb_obtener_registro(tabla->pid, i);
            registro->algoritmo = reg->alg;
            registro->marco = reg->marco; 
            registro->modificado = reg->modificado;
           // registro->presente = 1;
            registro->tlb = 1;
        }

        if (registro->algoritmo < LRU_min) {
            LRU_min  = registro->algoritmo;
            pagina   = registro;
            n_pagina = i;
            
        }
    }
    if (pagina ==NULL)
{ 
     printf("error con lru fijo\n");
     abort();
 }
    //pagina->tlb;
    t_victima victima;    
    victima.marco      = pagina->marco;
    victima.modificado = pagina->modificado;
    victima.pid        = tabla->pid;
    victima.pagina     = n_pagina;
    victima.tlb        = pagina->tlb;

    pagina->tlb=0;
    pagina->presente = 0;
    pagina->modificado = 0;

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
        log_info(logger_memoria, "EL CLOCK ESTA EN EL FRAME %d", i);
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

        printf("Estoy adentro 0");
        printf("El proceso tiene %d paginas \n", list_size(tabla->tabla_pag));
        printf("El valor de i es: %d \n", i);
                    
        pag_t* reg = list_get(tabla->tabla_pag, i);
        if(reg->presente != 1 ) 
		{
			if(i+1 == tamanio) 
				i = 0;
			else
            i++;
        
			iteracion++;
			continue;
		}

        if (reg->tlb == 1) {
            tlb_t* registro_tlb = tlb_obtener_registro(tabla->pid, i);
            reg->algoritmo = registro_tlb->alg;
            reg->marco = registro_tlb->marco; 
            reg->modificado = registro_tlb->modificado;
           // reg->presente = 1;
           // reg->tlb = 1;
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

        if(reg->presente != 1 || reg->algoritmo == -1) 
		{
			if(i+1 == tamanio) 
				i = 0;
			else
            i++;
        
			iteracion++;
			continue;
		}
        tlb_t* registro_tlb;
        if (reg->tlb == 1) {
            registro_tlb = tlb_obtener_registro(tabla->pid, i);
            reg->algoritmo = registro_tlb->alg;
            reg->marco = registro_tlb->marco; 
            reg->modificado = registro_tlb->modificado;
           // reg->presente = 1;
           // reg->tlb = 1;
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
        else{
            reg->algoritmo = 0;
            if(reg->tlb == 1) registro_tlb->alg = 0;
        }

        if(i+1 == tamanio) 
            i = 0;
        else
            i++;
        
        iteracion++;
    }

    return -1;
}



void page_use(int pid, int marco, pag_t* p, int n_pagina, int codigo)
{
    if (p->tlb == 1) {
        tlb_t *reg = buscar_reg_en_tlb(pid, n_pagina);
        if(reg == NULL){
            printf("Error con la pagina %d y el pid %d\n", n_pagina, pid);
            printf("No esta en la tlb\n");
            abort();
        }
        tlb_page_use(reg);

        if (strcmp(configuracion.ALGORITMO_REEMPLAZO_TLB, "LRU") == 0) reg->alg = alg_comportamiento_tlb();// que carajos??

        if (codigo == WRITE) {
            reg->modificado = WRITE;
        }
        return;
    }

    tlb_insert_page(pid, n_pagina, marco, codigo);

    if (configuracion.CANTIDAD_ENTRADAS_TLB > 0) p->tlb = 1;
    if (codigo == WRITE) p->modificado = WRITE;
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
    if (configuracion.CANTIDAD_ENTRADAS_TLB == 0) return; 
    int victima = tlb_obtener_victima();
    
    printf("La victima de la tlb es %d\n", victima);
    log_info(logger_memoria,"La victima de la tlb es %d\n", victima);
    tlb_t* reg = list_get(tlb, victima);
    log_info(logger_memoria,"La victima del reemplazo es %d del carpincho %d", reg->pagina, reg->pid);
    log_info(logger_memoria,"Se reemplazara por %d del carpincho %d", n_pagina, pid); // checkear
    
    log_info(logger_tlb, "Victima: PID %d | PAG %d | REG TLB %d | MARCO %d", reg->pid, reg->pagina, victima, reg->marco);

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

void tlb_actualizar_entrada_vieja(int pid, int pagina)
{
   int tamanio = list_size(tlb);
    for (int i=0; i < tamanio; i++) {

        tlb_t *reg = list_get(tlb, i);
        if(reg->pid == pid  && reg->pagina == pagina) {
            reg->pid = -1;
        }
    }
}

pag_t * pagina_obtener(int pid, int pagina)
{
    tab_pags *tabla = buscar_page_table(pid);
    pag_t *pag = list_get(tabla->tabla_pag, pagina);

    return pag;
}


tlb_t* pagina_obtener_datos_de_tlb(int pid, int pagina, pag_t *pag)
{
    if (configuracion.CANTIDAD_ENTRADAS_TLB == 0) return NULL;
    if (pag->tlb == 0) return NULL;

    tlb_t *tlb_reg = tlb_obtener_registro(pid, pagina);

    pag->algoritmo = tlb_reg->alg;
    pag->modificado = tlb_reg->modificado;
    pag->marco = tlb_reg->marco;

    return tlb_reg;
}

void clock_puntero_actualizar_fija(int *posicion, int marcos_asignados)
{
    if (*posicion + 1 == marcos_asignados) {
        *posicion = 0;
    } else {
        *posicion = *posicion + 1;
    }
}
int marcos_asginados_a_proceso(tab_pags* tabla)
{
   int cant = 0;
   int tamanio = list_size(tabla->tabla_pag);

   for (int i = 0; i < tamanio; i++) {
       pag_t *pag = list_get(tabla->tabla_pag, i);
       if (pag->presente == 1) cant++;
   }

   return cant;
}
int pagina_siguiente_en_memoria_por_pid(tab_pags *tabla)
{
    marco_t *marco_final = NULL;

    while ( marco_final == NULL) {
        marco_t* marco = list_get(marcos, tabla->p_clock);
        if( marco->pid == tabla->pid) marco_final = marco;

        if(tabla->p_clock + 1 == list_size(marcos)){
            tabla->p_clock = 0;
        } else {
            tabla->p_clock = tabla->p_clock + 1;
        }

    }

    return marco_final->pagina;
}
pag_t *buscar_00_cf(tab_pags* tabla, int* pagina)
{

    int posicion = tabla->p_clock;
    int iteracion = 0;
    int marcos_asignados = marcos_asginados_a_proceso(tabla);

    pag_t* pagina_victima = NULL;

    while ( iteracion < marcos_asignados && pagina_victima == NULL) { 

        int c_pagina = pagina_siguiente_en_memoria_por_pid(tabla);
        pag_t *pag = pagina_obtener(tabla->pid, c_pagina);
        
        pagina_obtener_datos_de_tlb(tabla->pid, c_pagina, pag);

        if (pag->algoritmo == 0 && pag->modificado == 0) {
            pagina_victima = pag;
            *pagina = c_pagina;
        }

        // clock_puntero_actualizar_fija(&posicion, marcos_asignados);
        iteracion++;
    }

    return pagina_victima;
}

pag_t *buscar_01_cf(tab_pags* tabla, int* pagina)
{

    int posicion = tabla->p_clock;
    int iteracion = 0;
    int marcos_asignados = marcos_asginados_a_proceso(tabla);

    pag_t* pagina_victima = NULL;

    while ( iteracion < marcos_asignados && pagina_victima == NULL) { 

        int c_pagina = pagina_siguiente_en_memoria_por_pid(tabla);
        pag_t *pag = pagina_obtener(tabla->pid, c_pagina);

        tlb_t *tlb_reg = pagina_obtener_datos_de_tlb(tabla->pid, c_pagina, pag);

        if (pag->algoritmo == 0 && pag->modificado == 1) {
            pagina_victima = pag;
            *pagina = c_pagina;
        } else {
            pag->algoritmo = 0;
            if (pag->tlb == 1) tlb_reg->alg = 0;
        }

        // clock_puntero_actualizar_fija(&posicion, marcos_asignados);
        iteracion++;
    }

    return pagina_victima;
}

t_victima clock_fijo(int pid, tab_pags* tabla)
{
    int victima_pagina;
    pag_t *pagina = NULL;

    while (pagina == NULL) {
        
        pagina = buscar_00_cf(tabla, &victima_pagina);
        if( pagina != NULL) break;
        
        pagina = buscar_01_cf(tabla, &victima_pagina);
    }

    t_victima victima;
    victima.pid        = tabla->pid;
    victima.pagina     = victima_pagina;
    victima.marco      = pagina->marco;
    victima.modificado = pagina->modificado;
    victima.tlb        = pagina->tlb;

    log_info(logger_clock, "El elegido por clock fue: PID = %d - PAG = %d", victima.pid, victima.pagina);

    pagina->presente = 0;
    pagina->modificado = 0;
    pagina->tlb = 0;

    return victima;
}

void clock_puntero_actualizar_2(int *posicion)
{ 
        if (*posicion+1 == list_size(marcos)) {
            *posicion = 0;
        } else {
            *posicion = *posicion + 1;
        }
}
pag_t *buscar_00(int* pid, int *pagina)
{
    int posicion = clock_puntero;
    int iteracion = 0;

    pag_t* pagina_victima = NULL;

    while ( iteracion < list_size(marcos) && pagina_victima == NULL) { 

        marco_t *marco = list_get(marcos, posicion);
        pag_t *pag = pagina_obtener(marco->pid, marco->pagina);
        
        pagina_obtener_datos_de_tlb(marco->pid, marco->pagina, pag);

        if (pag->algoritmo == 0 && pag->modificado == 0) {
            pagina_victima = pag;
            *pid = marco->pid; 
            *pagina = marco->pagina;
        }

        clock_puntero_actualizar_2(&posicion);
        iteracion++;
    }

    clock_puntero = posicion;

    return pagina_victima;
}

pag_t *buscar_01(int* pid, int *pagina)
{
    int posicion = clock_puntero;
    int iteracion = 0;

    pag_t* pagina_victima = NULL;

    while ( iteracion < list_size(marcos) && pagina_victima == NULL) { 

        marco_t *marco = list_get(marcos, posicion);
        pag_t *pag = pagina_obtener(marco->pid, marco->pagina);
        
        tlb_t *tlb_reg = pagina_obtener_datos_de_tlb(marco->pid, marco->pagina, pag);

        if (pag->algoritmo == 0 && pag->modificado == 1) {
            pagina_victima = pag;
            *pid = marco->pid; 
            *pagina = marco->pagina;
        } else {
            pag->algoritmo = 0;
            if (pag->tlb == 1) tlb_reg->alg = 0;
        }

        clock_puntero_actualizar_2(&posicion);
        iteracion++;
    }

    clock_puntero = posicion;

    return pagina_victima;
}


t_victima cd(int pid, tab_pags* tabla)
{
    int victima_pid;
    int victima_pagina;
    pag_t *pagina = NULL;

    while (pagina == NULL) {
        
        pagina = buscar_00(&victima_pid, &victima_pagina);
        if( pagina != NULL) break;
        
        pagina = buscar_01(&victima_pid, &victima_pagina);
    }

    t_victima victima;
    victima.pid        = victima_pid;
    victima.pagina     = victima_pagina;
    victima.marco      = pagina->marco;
    victima.modificado = pagina->modificado;
    victima.tlb        = pagina->tlb;

    log_info(logger_clock, "El elegido por clock fue: PID = %d - PAG = %d", victima.pid, victima.pagina);

    pagina->presente = 0;
    pagina->modificado = 0;
    pagina->tlb = 0;

    return victima;
}
