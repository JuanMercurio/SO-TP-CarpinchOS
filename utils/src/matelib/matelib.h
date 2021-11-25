#ifndef _MATELIB_H_
#define _MATELIB_H_

#include "../mensajes/mensajes.h"

#include <commons/collections/list.h>
#include <commons/config.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define INIT 0
#define NOT_ASIGNED -1

//-------------------Operations Codes For Messages---------/

typedef enum{
    NEW_INSTANCE,
    IO,
    INIT_SEMAFORO,
    SEM_WAIT,
    SEM_POST,
    SEM_DESTROY,
    MEMALLOC,
    MEMFREE,
    MEMREAD,
    MEMWRITE,
    MATE_CLOSE
}cod_op;

typedef enum{
    MATE_FREE_FAULT = -5,
    MATE_READ_FAULT = -6,
    MATE_WRITE_FAULT = -7,
}mate_errors;

//-------------------Type Definitions----------------------/

typedef struct mate_inner_structure{
    uint32_t pid;
    uint32_t conexion; 
    uint32_t conectado_a;
    //sem_t* sem_instance;
    //void* memoria;
} mate_inner_structure;

typedef struct mate_instance {
    void* group_info;
} mate_instance;

typedef char *mate_io_resource;

typedef char *mate_sem_name;

typedef int32_t mate_pointer;

//------------------General Functions---------------------/
void mate_init(mate_instance *lib_ref, char *config);

int mate_close(mate_instance *lib_ref);

//-----------------Semaphore Functions---------------------/
int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value);

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem);

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem);

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem);

//--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg);

//--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size);

int mate_memfree(mate_instance *lib_ref, mate_pointer addr);

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size);

int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size);

void* leer_bloque(void* bloque, int offset, int tamanio);

typedef enum{
  PID, SOCKET, SERVER,
}cod_int;

int obtener_int(mate_instance* lib_ref, cod_int COD);

void* crear_mate_inner(int pid, int conexion, int server);

void conexion_success(int pid);

void solicitud_malloc(int socket, int pid, int size);

mate_pointer recibir_dl(int socket);

mate_pointer recibir_int32(int socket);

bool conectado_a_memoria(mate_instance* lib_ref);

#endif