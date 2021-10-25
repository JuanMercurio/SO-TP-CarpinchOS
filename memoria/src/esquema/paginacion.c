#include "paginacion.h"
#include "../configuracion/config.h"
#include "algoritmos.h"

#include <stdlib.h>
#include <stdio.h>

void iniciar_paginacion(){

    ids_memoria = 0;
    init_ram();
    init_bitmap_frames();
    set_asginacion();
}

void init_ram(){
    ram.memoria = malloc(configuracion.TAMANIO);
    pthread_mutex_init(&ram.mutex, NULL);
}

void init_bitmap_frames(){
    int n_frames = configuracion.TAMANIO / configuracion.TAMANIO_PAGINAS;
    marcos = calloc(n_frames, sizeof(int));
}

void set_asignacion(){
    if(strcmp(configuracion.TIPO_ASIGNACION, "FIJA"))
    {
        lru = &lru_fijo;
    }
    else
    {
        lru = &lru_dinamico;
    }
}

void add_new_page_table(tab_pags* tabla){
    pthread_mutex_lock(&tablas.mutex);
    list_add(tablas.lista, tabla);
    pthread_mutex_unlock(&tablas.mutex);
}

tab_pags* buscar_page_table(int pid){ 
    for(int i=0; i<list_size(tablas.lista); i++){
        tab_pags* tabla = list_get(tablas.lista, i);
        if(tabla->pid = pid) return tabla;
    }
    return NULL;
}