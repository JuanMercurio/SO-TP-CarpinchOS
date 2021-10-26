#include "operaciones.h"
#include "../esquema/paginacion.h"
#include "../configuracion/config.h"

#include <string.h>
#include <stdlib.h>

#define SIZE_METADATA sizeof(HeapMetadata)

int  memalloc(int tamanio, int pid){ //quizas al igual que antes, el carpincho se guarda local en el hilo? entonces no habría que pasarlo como parametro

	tabla_paginas = obtener_tabla_paginas(pid);
	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
	int inicio_actual = 0;

	do{
		if(ptr_potencial_segmento->isFree && ptr_potencial_segmento->nextAlloc - SIZE_METADATA - inicio_actual >= tamanio) {
			//encontre el segmento donde entra
			//ahora debo evaluar si entra justo o si tengo que dividirlo en 2 partes

			if(ptr_potencial_segmento->nextAlloc - SIZE_METADATA - inicio_actual == tamanio){
				//entra justo
				ptr_potencial_segmento->isFree = false;
				return inicio_actual; //retorno la direccion logica
			}

			//tengo que dividir en 2                   			DUDA: que pasa si el lugar que queda es menor al tamaño de un metadata?
			ptr_potencial_segmento->isFree = false;
			int aux_next_alloc = ptr_potencial_segmento->nextAlloc;
			ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;

			HeapMetadata* new;
			new->prevAlloc = inicio_actual;
			new->nextAlloc = aux_next_alloc;
			new->isFree = true;

			memcpy(memoria_virtual + ptr_potencial_segmento->nextAlloc, new, sizeof(new));

			HeapMetadata* next = memoria_virtual + aux_next_alloc; //quizas esta mal
			next->prevAlloc = ptr_potencial_segmento->nextAlloc; //el prevAlloc es igual al nextAlloc de 2 metadatas atras
		}

		//Si no entre al if anterior, significa que no encontre un hueco todavia. Tengo que pasar al siguiente alloc.
		ptr_potencial_segmento += ptr_potencial_segmento->nextAlloc; //TODO: REVISAR

	} while(ptr_potencial_segmento->nextAlloc != NULL);

	// no hay espacio en ninguna pagina

	//comportamientos distintos: a. si "se genera un nuevo alloc al final del espacio de direcciones" / b. se deniega la operación porque no hay más espacio
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
