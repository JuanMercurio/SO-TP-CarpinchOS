#ifndef _SIGNALS_USUARIO_
#define _SIGNALS_USUARIO_

#include "../esquema/estructuras.h"
#include "../esquema/tlb.h"

extern bool ejecutando;
extern int server;

/*
    @NAME: signal_init
    @DESC: Crea las estrucuras para usar sigaction()
           Selecciona el handler para la signal elegida
 */
void signal_init(int sig, void(*handler)); 

/* 
    @NAME: iniciar_signals
 */
void iniciar_signals();

/* Handlers */
void imprimir_TLB();
void generar_dump();
void limpiar_TLB();
void programa_terminar_forzado();
    
void tabla_actualizar(tlb_t* reg);
void tlb_imprimir_reg(tab_pags* reg);

char* itos(int n);
void tlb_print_on_file(FILE* tlb);
#endif
