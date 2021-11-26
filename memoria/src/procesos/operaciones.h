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
}__attribute__((packed)) HeapMetadata;

HeapMetadata* primer_segmento(tab_pags* tabla);
void* ubicacion_nuevo_segmento(int num_marco, int dir_log);
int num_pagina_a_traer(uint32_t inicio);
bool hay_cambio_de_pagina(int direc_actual, uint32_t next_alloc);
int cant_cambios_de_pagina(int direc_actual, uint32_t next_alloc);

//CAMBIADO
HeapMetadata* hallar_metadata(uint32_t dir_log, tab_pags* tabla);//, int* num_pag);

bool pedir_memoria_a_swap(int tamanio);

int memalloc(int tamanio, int pid);
int memfree(int dir_log, int pid);

void TEST_agregar_nueva_pagina(tab_pags* tabla, int marco);
void TEST_report_metadatas(int pid);

bool read_verify_size(tab_pags* t, dir_t dl, int tamanio);
int read_get_readable_bytes(dir_t dl, int count);
int memoria_escribir(tab_pags* tabla, dir_t dl, void* contenido, int tamanio);
void* memoria_leer(tab_pags* tabla, dir_t dl, int tamanio);
int memoria_escribir_por_dirlog(tab_pags* tabla, int dl, void* contenido, int tamanio);
void* memoria_leer_por_dirlog(tab_pags* tabla, int dl, int tamanio);

#endif
