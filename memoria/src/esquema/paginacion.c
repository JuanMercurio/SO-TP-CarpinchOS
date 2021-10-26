#include "paginacion.h"
#include "../configuracion/config.h"
#include "algoritmos.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

void iniciar_paginacion(){

    ids_memoria = 0;
    init_ram();
    init_bitmap_frames();
    set_asignacion();
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
    if(strcmp(configuracion.TIPO_ASIGNACION, "FIJA") == 0)
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
        if(tabla->pid == pid) return tabla;
    }
    return NULL;
}

dir_t traducir_dir_log(int dir_log){
     dir_t dir;
     return dir;
}

dir_t convertir_a_df(t_list* tabla, dir_t dl){
    dir_t df;
    pag_t* reg = list_get(tabla, dl.PAGINA);
    df.offset = dl.offset;
    df.MARCO = reg->marco;
    return df;
}

int offset_memoria(dir_t df){
    return df.MARCO * configuracion.TAMANIO_PAGINAS + df.offset;
}