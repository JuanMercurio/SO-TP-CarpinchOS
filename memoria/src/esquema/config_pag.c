#include "config_pag.h"

#include <mensajes/mensajes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int swap;

int (*algoritmo_tlb)();
void (*tlb_page_use)(tlb_t* reg);
void (*page_use)(int, tab_pags*);
t_victima (*algoritmo_mmu)(int, tab_pags*);

void set_algoritmos(){
    set_alg_tlb();
    set_alg_principal();
}

void set_alg_tlb(){

    if ( strcmp(configuracion.ALGORITMO_REEMPLAZO_TLB, "FIFO") == 0 ) { algoritmo_tlb = fifo_tlb; return;}
    if ( strcmp(configuracion.ALGORITMO_REEMPLAZO_TLB, "LRU")  == 0 ) { algoritmo_tlb = lru_tlb;  return;}

    printf("Configuracion Invalida en ~/memoria/cgf/memoria.config:  \n\n");
    printf("    - ALGORITMO_REEMPLAZO_TLB=%s --> Elegir entre FIFO y LRU \n\n", configuracion.ALGORITMO_REEMPLAZO_TLB);

    abort();
}

void set_alg_principal(){

    if ( strcmp(configuracion.TIPO_ASIGNACION, "FIJA") == 0 && \
         strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "CLOCK-M") == 0)        
       { 
          algoritmo_mmu = clock_fijo;
          page_use = page_use_clock_modificado;
          return;
       }
                                                                               

    if ( strcmp(configuracion.TIPO_ASIGNACION, "DINAMICA") == 0 && \
         strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "CLOCK-M") == 0)          { algoritmo_mmu = clock_dinamico; return; }

    if ( strcmp(configuracion.TIPO_ASIGNACION, "FIJA") == 0 && \
         strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "LRU") == 0)              { algoritmo_mmu = lru_fijo; return; }

    if ( strcmp(configuracion.TIPO_ASIGNACION, "DINAMICA") == 0 && \
         strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "LRU") == 0)              { algoritmo_mmu = lru_dinamico; return; }
         
    printf("Configuracion Invalida en ~/memoria/cgf/memoria.config:  \n\n");
    printf("    - ALGORITMO_REEMPLAZO_MMU=%s --> Elegir entre CLOCK-M y LRU \n", configuracion.ALGORITMO_REEMPLAZO_MMU);
    printf("    - TIPO_ASIGNACION=%s --> Elegir entre FIJA y DINAMICA \n\n",     configuracion.TIPO_ASIGNACION);

    abort();
}

int set_campo_algoritmo(){
    if(strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "CLOCK-M") == 0) return 0;
    if(strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "LRU") == 0) return LRU_C;
    printf("Algoritmo de MMU erroneo\n");
    printf("Terminando el programa");
    abort();
}


void asignacion_swap(){
     enviar_mensaje(swap, configuracion.TIPO_ASIGNACION);
}

void swap_conexion_init(int server){
     swap = aceptar_cliente(server);
     asignacion_swap();
}