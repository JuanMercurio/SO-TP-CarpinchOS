#include "signal.h"

#include "../esquema/tlb.h"

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

bool ejecutando = true;
int server;

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
    signal_init(SIGTSTP, &programa_terminar_forzado);
}

/* Las siguientes funciones pueden ser transferidas a otro lugar para mas comodidad */

void imprimir_TLB(){
    
    printf("\n");
    printf("Total TLB Hits: %d\n", TLB_HIT_COUNT);
    printf("Total TLB Misses: %d\n", TLB_MISS_COUNT);
    printf("\n");

    int tamanio = list_size(tlb_information);
    for (int i=0; i < tamanio; i++) {
        tlb_info_t* element = list_get(tlb_information, i);
        printf("PID: %d - TLB MISS: %d - TLB HITS: %d\n", element->pid, element->miss, element->hits);
    }
    /* int tamanio = list_size(tablas.lista); */
    /* for(int i=0; i < tamanio; i++) */
    /* { */
    /*     tab_pags* reg = list_get(tlb, i); */
    /*     tlb_imprimir_reg(reg); */
    /* } */
}

void generar_dump(){

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char* separator = "------------------------------------------------------------------------------\n";
    char sdate[20];
    char shour[20];
    char path[50];
    char title[70];

    sprintf(sdate, "%d-%d-%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    sprintf(shour, "%d:%d:%d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(path, "%s%s%s%s%s\n", configuracion.PATH_DUMP_TLB, "Dump_", sdate, ".", shour) ;
    sprintf(title, "Dump: %s %s\n", sdate, shour);

    FILE* dump_file = fopen(path, "w+");

    fwrite(separator, strlen(separator), 1, dump_file);
    fwrite(title, strlen(title), 1, dump_file);

    tlb_print_on_file(dump_file);

    fwrite(separator, strlen(separator), 1, dump_file);

    fclose(dump_file);
}

void tlb_print_on_file(FILE* dump_file)
{ 
    int tlb_size = list_size(tlb);

    for(int i=0; i < tlb_size; i++){

       tlb_t* reg = list_get(tlb, i);

        int entrada = i;
        char* pagina;
        char* marco;
        char* pid;
        char* estado;

       if(reg->pid == TLB_EMPTY)
       {
           pagina = "-";
           marco = "-";
           pid = "-";
           estado = "Libre";
       }

       else
       {
           pagina = itos(reg->pagina);
           marco = itos(reg->marco);
           pid = itos(reg->pid);
           estado = "Ocupado";
       }

        char registro[100];
       sprintf(registro, "Entrada: %-4d   Estado: %-6s     PID: %-3s   Pagina: %-4s   Marco: %-4s\n", entrada, estado , pid, pagina, marco);
       free(pagina);
       free(marco);
       free(pid);
       free(estado);
       fwrite(registro, strlen(registro), 1, dump_file);
    }

}

char* itos(int n){
    int num = n;
    int i = 0;
    while(n!=0){
        n /= 10;
        i++;
    } 

    char* s = malloc(i++);
    sprintf(s, "%d", num);

    return s;
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

void programa_terminar_forzado()
{
	// int pid = getpid();
    ejecutando = false;
    close(server);
//	kill(pid, SIGTERM);
}

/* Funciones adicionales */

void tlb_imprimir_reg(tab_pags* reg)
{
    printf( "PID: %d  |  TLB Hits: %d  |  TLB Misses: %d \n", reg->pid, reg->TLB_HITS, reg->TLB_MISSES);
}

void tabla_actualizar(tlb_t* reg)
{
    if (reg->pid == -1) return;
    pag_t *pag = pagina_obtener(reg->pid, reg->pagina);
    pag->algoritmo = reg->alg;
    pag->modificado = reg->modificado;
    pag->tlb = 0;
}