#ifndef _OPERACIONES_MEMORIA_
#define _OPERACIONES_MEMORIA_

#include <stdint.h>
#include <commons/collections/list.h>

#define PID_EMPTY 0 

/* Estrucutra de los alloc */
typedef struct HeadMetaData{ 
    uint32_t prevAlloc;
    uint32_t nextAlloc;
    uint8_t isFree;
}HeadpMetaData;

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
    @NAME:  crear_pagina
    @DESC:  crea una pagina en la ultima posicion de la tabla
 */
void crear_pagina(t_list* tabla);

/*
    @NAME:  alloc_memory
    @DESC:  aloca la memoria y retorna la direccion logica
 */
uint32_t alloc_memory(t_list* tabla, int size);

#endif