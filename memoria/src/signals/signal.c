#include "signal.h"

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
    printf("Imprimir TLB \n");
}

void generar_dump(){
    printf("Generando DUMP \n");
}

void limpiar_TLB(){
    printf("Limpiando TLB \n");
}