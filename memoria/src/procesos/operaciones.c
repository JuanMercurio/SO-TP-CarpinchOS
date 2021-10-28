#include "operaciones.h"
#include "../esquema/paginacion.h"
#include "../configuracion/config.h"

#include <mensajes/mensajes.h>
#include <utils/utils.h>
#include <string.h>
#include <stdlib.h>

uint32_t memalloc(t_list* tabla, int cliente){

   int size = recibir_tamanio_malloc(cliente);
   if(list_size(tabla) == 0) crear_pagina(tabla); 
   uint32_t dl = alloc_memory(tabla, size);

   return dl;
}

uint32_t alloc_memory(t_list* tabla, int size){
   return 0;   
}


void crear_pagina(t_list* tabla){

}

int recibir_tamanio_malloc(int cliente){
    return recibir_tamanio(cliente);
}

// int desginar_PID(int cliente){
//     int pid = recibir_PID(cliente);
//     if(pid == PID_EMPTY) return crearID(&ids_memoria);
//     return pid;
// }

int recibir_PID(int cliente){
    return recibir_tamanio(cliente);
}

void ejecutar_malloc(t_list* tabla, int cliente){

    int size = recibir_tamanio_malloc(cliente);

    if(list_size(tabla) == 0) crear_pagina(tabla); 
    alloc_memory(tabla, size);
}

void memfree();

void* memread(tab_pags* tabla, int dir_log, int tamanio){

   dir_t dl = traducir_dir_log(dir_log);
   dir_t df = convertir_a_df(tabla->tabla_pag, dl);

   int leido = 0;
   int size_in_page = configuracion.TAMANIO_PAGINAS - tamanio;

   void* buffer = malloc(tamanio);

   while(leido<tamanio)
   {
     if(size_in_page > tamanio)
     {
        memcpy(buffer + leido, ram.memoria + offset_memoria(df), tamanio);
     }
   }

   return NULL;
}


void memwrite();

