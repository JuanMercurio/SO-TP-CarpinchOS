#ifndef _ESQUEMA_MEMORIA_
#define _ESQUEMA_MEMORIA_

#include <commons/collections/list.h>

#define TLB_MISS -1
#define TLB_EMPTY -1 

/* TLB */
t_list * tlb;

/* Registros de la tlb */
typedef struct tlb_t{
    int pid;
    int pagina;
    int marco;
}tlb_t;

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
int buscar_en_tlb(int pid, int pagina);

#endif