#ifndef _SIGNALS_USUARIO_
#define _SIGNALS_USUARIO_

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
    
#endif