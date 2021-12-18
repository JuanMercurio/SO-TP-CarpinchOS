#ifndef ESTRUCTURAS_MEMORIA_H
#define ESTRUCTURAS_MEMORIA_H

#include <pthread.h>
#include <commons/collections/list.h>

/* Lista de tablas de paginas y su mutex */
typedef struct memoria_t {
    char* memoria;
    pthread_mutex_t mutex;
}memoria_t;

/* Registros de la tlb */
typedef struct tlb_t{
    int pid;
    int pagina;
    int marco;
    int modificado;
    int alg_tlb;
    int alg;
}tlb_t;

/* Estructura para la supertabla de tablas  */
typedef struct tablas_t{
    t_list* lista;
    pthread_mutex_t mutex;
}tablas_t;

/* Esructura para tablas */
typedef struct tab_pags{
    int pid;
    int TLB_HITS;
    int TLB_MISSES;
    int p_clock;
    t_list* tabla_pag;
}tab_pags;

/* Registros de las tablas de paginas */
typedef struct pag_t{
    int marco;
    int presente;
    int modificado;
    int algoritmo;
    int tlb;
}pag_t;


/* Estructura para direccionamiento */
typedef struct dir_t{
   int segmento;
   int offset;
}dir_t;


typedef struct marco_t {
    int pid;
    int pagina;
}marco_t;

typedef struct t_victima{
    int pid;
    int pagina;
    int marco;
    int modificado;
    int tlb;
}t_victima;

/* Estructura Memoria */
extern tablas_t tablas;

typedef struct tlb_info_t {
    int pid;
    int miss;
    int hits;
}tlb_info_t;

#endif