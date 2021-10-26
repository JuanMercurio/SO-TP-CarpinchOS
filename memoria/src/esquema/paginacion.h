#ifndef _ESQUEMA_PAGINACION_
#define _ESQUEMA_PAGINACION_

#include <commons/collections/list.h>
#include <pthread.h>
#include <stdint.h>
#include <math.h>

#define PAGINA segmento
#define MARCO segmento

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

typedef struct dir_t{
   int segmento;
   int offset;
}dir_t;

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

/*
    @NAME:  set_asignacion
    @DESC:  segun el config les da valoes a las funciones puntero
 */
void set_asignacion();
/*
    @name:  obinario_a_decimal
    @desc:  dado un binario retorna su decimal
 */
int binario_a_decimal(uint32_t binario);

/*
    @NAME:  buscar_page_table
    @DESC:  segun un pid busca su tabla de paginas
            no es thread safe
 */

tab_pags* buscar_page_table(int pid);
/*
    @NAME:  offset_memoria
    @DESC:  dada una dir_t fisica retorna el byte en memoria
 */
uint32_t crear_dl(dir_t dl);

/*
    @NAME:  traducir_dir_log
    @DESC:  reduce una direccion logica en un dir_t logico
 */
dir_t traducir_dir_log(uint32_t dir_log);

/*
    @NAME:  convertir_a_df
    @DESC:  Dado una tabla y un dir_t, retorna un dir_t fisico
 */
dir_t convertir_a_df(t_list* tabla, dir_t dl);

/*
    @NAME:  offset_memoria
    @DESC:  dada una dir_t fisica retorna el byte en memoria
 */
int offset_memoria(dir_t df);

/*
    @name:  decimal_a_binario
    @desc:  dado un decimal retorna su binario
 */
uint32_t decimal_a_binario(int decimal);

/*
    @NAME:  oget_offset
    @DESC:  retorna el desplazamiento de un dl binario
 */
int get_offset(uint32_t dl, int multiplier);

#endif
