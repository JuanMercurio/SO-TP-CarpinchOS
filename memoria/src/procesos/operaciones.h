#ifndef _OPERACIONES_MEMORIA_
#define _OPERACIONES_MEMORIA_


#include <stdint.h>
#include <commons/collections/list.h>
#include "../esquema/paginacion.h"

#define PID_EMPTY 0 

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

HeapMetadata* hallar_metadata(uint32_t dir_log, tab_pags* tabla, int* num_pag);

bool pedir_memoria_a_swap(int tamanio);

int memalloc(int tamanio, int pid);
int memfree(int dir_log, int pid);

void TEST_agregar_nueva_pagina(tab_pags* tabla, int marco);
void TEST_report_metadatas(int pid);

/*
    @NAME:  idesginar_PID
    @DESC:  Designa el pid del proceso
 */
int desginar_PID(int cliente);

/*
    @NAME:  recibir_PID
    @DESC:  Recibe el pid que le manda un carpincho o el kernel
 */
int recibir_PID(int cliente);

/*
    @NAME:  recibir_tamanio_malloc
    @DESC:  recibe el tamanio a alocar
 */
int recibir_tamanio_malloc(int cliente);

/*
    @NAME:  idesginar_PID
    @DESC:  Designa el pid del proceso
 */
int desginar_PID(int cliente);


/*
    @NAME:  alloc_memory
    @DESC:  aloca la memoria y retorna la direccion logica
 */
uint32_t alloc_memory(t_list* tabla, int size);

#endif
