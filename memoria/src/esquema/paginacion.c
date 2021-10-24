#include "paginacion.h"
#include "../configuracion/config.h"
#include <stdlib.h>
#include <stdio.h>

void iniciar_paginacion(){

    init_ram();
    init_bitmap_frames();
}

void init_ram(){
    ram.memoria = malloc(configuracion.TAMANIO);
    pthread_mutex_init(ram.mutex_mem, NULL);
}

void init_bitmap_frames(){
    int n_frames = configuracion.TAMANIO / configuracion.TAMANIO_PAGINAS;
    marcos = calloc(n_frames, sizeof(int));
}