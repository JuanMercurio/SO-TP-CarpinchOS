#include "signal.h"

#include "../esquema/tlb.h"

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void signal_init(int sig, void(*handler)){ 
    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    sigaction(sig, &sa, NULL);
}

void iniciar_signals(){
    signal_init(SIGINT, &imprimir_TLB);
    signal_init(SIGUSR1, &generar_dump);
    signal_init(SIGUSR2, &limpiar_TLB);
}

/* Las siguientes funciones pueden ser transferidas a otro lugar para mas comodidad */

void imprimir_TLB(){

    printf("Total TLB Hits: %d\n", TLB_HIT_COUNT);
    printf("Total TLB Misses: %d\n", TLB_MISS_COUNT);
    printf("\n");

    int tamanio = list_size(tablas.lista);
    for(int i=0; i < tamanio; i++)
    {
        tab_pags* reg = list_get(tlb, i);
        tlb_imprimir_reg(reg);
    }
}

void generar_dump(){
    printf("Generando DUMP \n");
}

void limpiar_TLB(){
    printf("Limpiando TLB \n");

    int tamanio = list_size(tlb);

    for(int i=0; i < tamanio; i++)
    {
        tlb_t* reg = list_get(tlb, i);
        tabla_actualizar(reg);

        reg->alg_tlb    = alg_tlb;
        reg->marco      = TLB_EMPTY;
        reg->pagina     = TLB_EMPTY;
        reg->pid        = TLB_EMPTY;
        reg->modificado = 0;
    }
}


/* Funciones adicionales */

void tlb_imprimir_reg(tab_pags* reg)
{
    printf( "PID: %d  |  TLB Hits: %d  |  TLB Misses: %d", reg->pid, reg->TLB_HITS, reg->TLB_MISSES);
}



void tabla_actualizar(tlb_t* reg)
{
     
}