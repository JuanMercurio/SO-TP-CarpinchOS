#include "operaciones.h"
#include "../esquema/paginacion.h"
#include "../configuracion/config.h"

#include <string.h>
#include <stdlib.h>

int  memalloc(int tamanio, int pid){

    /* Asegurarnos que entra en memoria:
            1) Buscamos si hay un alloc free en 
               el cual entre (usando First Fit) --> Dividimos el alloc en 2 --> el ocupado y el free 
            2) Si no encuentra entonces creamos un alloc al final, si no hay espacio pido una pagina nueva. || Este renglon ni idea 
            3) Tambien hay que ir a buscar a swamp
    */
   return 1;    //retorna la direccion logica a el inicio del bloque (no del alloc) -- Si no encontro retorna un puntero null
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