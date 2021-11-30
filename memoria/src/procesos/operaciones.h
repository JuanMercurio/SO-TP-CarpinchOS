#ifndef _OPERACIONES_MEMORIA_
#define _OPERACIONES_MEMORIA_

#include "../esquema/paginacion.h"
#include <matelib/matelib.h>

#include <commons/collections/list.h>
#include <stdint.h>

#define PID_EMPTY 0 
#define LAST_METADATA -1
#define FIRST_METADATA -2


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

int memalloc(tab_pags* tabla, int tamanio);
int memfree(tab_pags* tabla, int dir_log);

void TEST_agregar_nueva_pagina(tab_pags* tabla, int marco);
void TEST_report_metadatas(int pid);

bool read_verify_size(tab_pags* t, dir_t dl, int tamanio);
int read_get_readable_bytes(dir_t dl, int count);
int memoria_escribir(tab_pags* tabla, dir_t dl, void* contenido, int tamanio);
void* memoria_leer(tab_pags* tabla, dir_t dl, int tamanio);
int memoria_escribir_por_dirlog(tab_pags* tabla, int dl, void* contenido, int tamanio);
void* memoria_leer_por_dirlog(tab_pags* tabla, int dl, int tamanio);

void TEST_agregar_nueva_pagina(tab_pags* tabla, int marco);
void TEST_report_metadatas(int pid);

int memwrite(tab_pags* tabla, int dir_log, void* contenido, int tamanio);
void* memread(tab_pags* tabla, int dir_log, int tamanio);

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

/*
    @NAME:  memoria_leer
    @DESC:  lee ram segun un dir_t y tamanio
 */
void* memoria_leer(tab_pags* tabla, dir_t dl, int tamanio);

int memoria_escribir(tab_pags* tabla, dir_t dl, void* contenido, int tamanio);
/*
    @NAME:  read_verify_size
    @DESC:  retorna true si es posible leer el tamanio en la dl
 */
bool read_verify_size(tab_pags* t, dir_t dl, int tamanio);

/*
    @NAME:  read_get_readable_bytes
    @DESC:  retorna la cantidad de bytes que es posible leer 
            segun un dl
 */
int read_get_readable_bytes(dir_t dl, int count);

dir_t heap_get_location(dir_t dl, tab_pags* t);

bool alloc_valido(dir_t dl, tab_pags* t, int tamanio);

bool heap_lectura_valida(HeapMetadata* data, dir_t dl, int tamanio);

void heap_init(tab_pags* tabla);
    
#endif
