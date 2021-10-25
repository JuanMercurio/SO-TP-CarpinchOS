#ifndef _ESQUEMA_PAGINACION_
#define _ESQUEMA_PAGINACION_

#include <commons/collections/list.h>
#include <pthread.h>

/* Estructura Memoria */
typedef struct memoria_t {
    void* memoria;
    pthread_mutex_t mutex;
}memoria_t;

/* MEMORIA PRINCIPAL */
memoria_t ram;

typedef struct tablas_t{
    t_list* lista;
    pthread_mutex_t mutex;
}tablas_t;

/* Lista de tablas de paginas y su mutex */
tablas_t tablas;

/* Bitmap de marcos */
int* marcos; 

/* Esructura para tablas */
typedef struct tab_pags{
    int pid;
    t_list* tabla_pag;
}tab_pags;

/* Registros de las tablas de paginas */
typedef struct pag_t{
    int marco;
    int presente;
    int modificado;
    int algoritmo;
}pag_t;

/* Varibale para generar los id en memoria */
int ids_memoria;

/*
    @NAME:  iniciar_paginacion
    @DESC:  inicia todas las estructuras necesarias 
            para utilizar paginacion
 */
void iniciar_paginacion();

/*
    @NAME:  init_ram
    @DESC:  aloca la memoria principal y inicia el mutex
 */
void init_ram();

/*
    @NAME:  init_bitmap_frames
    @DESC:  iniciar el bitmap de frames de la memoria fisica
 */
void init_bitmap_frames();

/*
    @NAME:  add_new_page_table
    @DESC:  agrega un tab_pag* a la lista de todos ellos 
            Es thread safe
 */
void add_new_page_table(tab_pags*);

tab_pags* buscar_page_table(int pid);

#endif