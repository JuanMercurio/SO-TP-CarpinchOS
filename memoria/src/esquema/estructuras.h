#ifndef ESTRUCTURAS_MEMORIA_H
#define ESTRUCTURAS_MEMORIA_H

#include <pthread.h>
#include <commons/collections/list.h>


/* Lista de tablas de paginas y su mutex */
typedef struct memoria_t {
    void* memoria;
    pthread_mutex_t mutex;
}memoria_t;

/* Estructura para la supertabla de tablas  */
typedef struct tablas_t{
    t_list* lista;
    pthread_mutex_t mutex;
}tablas_t;

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


/* Estructura para direccionamiento */
typedef struct dir_t{
   int segmento;
   int offset;
}dir_t;

#endif