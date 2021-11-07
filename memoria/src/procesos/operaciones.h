#ifndef _OPERACIONES_MEMORIA_
#define _OPERACIONES_MEMORIA_

#include <stdint.h>
#include <commons/collections/list.h>
#include "../esquema/paginacion.h"

/* Estrucutra de los alloc */
typedef struct HeapMetadata{
    uint32_t prevAlloc;
    uint32_t nextAlloc;
    uint8_t isFree;
}HeapMetadata;

HeapMetadata* primer_segmento(tab_pags* tabla);
void* ubicacion_nuevo_segmento(int num_marco, int dir_log);
int num_pagina_a_traer(uint32_t inicio);
bool hay_cambio_de_pagina(int direc_actual, uint32_t next_alloc);
int cant_cambios_de_pagina(int direc_actual, uint32_t next_alloc);

#endif
