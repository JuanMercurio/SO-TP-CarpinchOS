#ifndef _ESQUEMA_MEMORIA_
#define _ESQUEMA_MEMORIA_

#include <commons/collections/list.h>

#define TLB_MISS -1

/* TLB */
t_list * tlb;

/* Registros de la tlb */
typedef struct t_tlb{
    int pid;
    int pagina;
    int marco;
}t_tlb;

/*
    @NAME:  buscar_en_tlb
    @DESC:  busca en la tlb el frame en el cual esta
            la pagina del proceso especificado.
            Si no encuentra la pagina retorna TLB_MISS (-1) 
 */
int buscar_en_tlb(int pid, int pagina);

#endif