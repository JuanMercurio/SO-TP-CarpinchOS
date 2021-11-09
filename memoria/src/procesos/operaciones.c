#include "operaciones.h"
#include "../configuracion/config.h"
#include "../esquema/paginacion.h"

#include <string.h>
#include <stdlib.h>

#define SIZE_METADATA sizeof(HeapMetadata)

int memalloc(int tamanio, int pid){ //quizas al igual que antes, el carpincho se guarda local en el hilo? entonces no habría que pasarlo como parametro

	tab_pags* tabla_paginas = buscar_page_table(pid);
	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
	int inicio_actual = 0, num_pagina_actual=0;
	do{
		if(ptr_potencial_segmento->isFree && ptr_potencial_segmento->nextAlloc - SIZE_METADATA*2 - inicio_actual >= tamanio) { //*2 porque tiene que entrar otra metadata mas
			//encontre el segmento donde entra
			//ahora debo evaluar si entra justo o si tengo que dividirlo en 2 partes

			if(ptr_potencial_segmento->nextAlloc - SIZE_METADATA*2 - inicio_actual == tamanio){
				//entra justo
				ptr_potencial_segmento->isFree = false;
				return inicio_actual; //retorno la direccion logica
			}

			//tengo que dividir en 2                   			DUDA: que pasa si el lugar que queda es menor al tamaño de un metadata?
			ptr_potencial_segmento->isFree = false;
			int aux_next_alloc = ptr_potencial_segmento->nextAlloc;
			ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;

			HeapMetadata new;
			new.prevAlloc = inicio_actual;
			new.nextAlloc = aux_next_alloc;
			new.isFree = true;

			HeapMetadata* next;
			pag_t* pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
			//si el nuevo metadata esta en otra pagina
			if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
				int num_pag_a_traer = num_pagina_actual + cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc);
				pag_t* pagina_a_traer = list_get(tabla_paginas->tabla_pag, num_pag_a_traer);

				//TODO: la cargo en memoria

				memcpy(ubicacion_nuevo_segmento(pagina_a_traer->marco, ptr_potencial_segmento->nextAlloc), &new, SIZE_METADATA);

				if(hay_cambio_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc)) { //caso borde, el segmento a cambiarle el prevalloc tambien esta en otra pagina
					pag_t* pagina_next = list_get(tabla_paginas->tabla_pag, num_pag_a_traer + cant_cambios_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc));
					//TODO: la cargo en memoria
					next = ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc);
				}
				else{
					next = ubicacion_nuevo_segmento(pagina_a_traer->marco, new.nextAlloc);
				}
				next->prevAlloc = ptr_potencial_segmento->nextAlloc;
				return inicio_actual;
			}

			if(hay_cambio_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc)){
				pag_t* pagina_next = list_get(tabla_paginas->tabla_pag, num_pagina_actual + cant_cambios_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc));
				//TODO: la cargo en memoria
				next = ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc);
			}
			else{
				next = ubicacion_nuevo_segmento(pagina->marco, new.nextAlloc);
			}
			next->prevAlloc = ptr_potencial_segmento->nextAlloc;

			memcpy(ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc), &new, SIZE_METADATA);

			return inicio_actual;
		}

		//Si no entre al if anterior, significa que no encontre un hueco todavia. Tengo que pasar al siguiente alloc.
		if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
			//El siguiente metadata esta en otra pagina
			//Obtengo dir fisica de inicio de pagina nueva, luego el metadata esta en [direccion.segmento + configuracion.TAMANIO_PAGINA - inicio_actual]
			num_pagina_actual += cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc);
			pag_t* pagina_a_traer = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
			//TODO: la cargo en memoria
			ptr_potencial_segmento = ubicacion_nuevo_segmento(pagina_a_traer->marco, (ptr_potencial_segmento->nextAlloc % configuracion.TAMANIO_PAGINAS)); //TODO: CHECKEAR LOGICA
			continue;
		}

		inicio_actual = ptr_potencial_segmento->nextAlloc;
		ptr_potencial_segmento += ptr_potencial_segmento->nextAlloc - inicio_actual; //TODO: REVISAR

	} while((ptr_potencial_segmento->nextAlloc) != 0); //0 vendria a ser NULL. ¿no falta una iteracion?

	// no hay espacio en ninguna pagina

	//comportamientos distintos: a. si "se genera un nuevo alloc al final del espacio de direcciones" / b. se deniega la operación porque no hay más espacio
	//se encarga el swap

	//tiene que pedir la cantida de paginas que necesita
	num_pagina_actual = list_size(tabla_paginas->tabla_pag);
	int inicio_anterior = inicio_actual;
	inicio_actual = num_pagina_actual * configuracion.TAMANIO_PAGINAS;

	if(pedir_memoria_a_swap(tamanio+SIZE_METADATA) == false){ //TODO: placeholder
		return NULL;
	}
	else{ //TODO: duda, todas las paginas que pido nuevas se encuentran en memoria?
		ptr_potencial_segmento->nextAlloc = inicio_actual;

		pag_t* primera_pagina_nueva = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
		HeapMetadata new;
		new.isFree = false;
		new.prevAlloc = inicio_actual;
		if(tamanio - SIZE_METADATA*2 % configuracion.TAMANIO_PAGINAS == 0){//entra justo
			new.nextAlloc=0; //TODO: NULL?
			memcpy(ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0), &new, SIZE_METADATA); //cargo la nueva metadata
		}
		else{
			//tengo que dividir en 2
			new.nextAlloc = inicio_actual + SIZE_METADATA + tamanio;
			memcpy(ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0), &new, SIZE_METADATA);

			int num_pagina_next = num_pagina_actual + cant_cambios_de_pagina(inicio_actual, new.nextAlloc);
			pag_t* pagina_next = list_get(tabla_paginas->tabla_pag, num_pagina_next);
			//hace falta traerla a memoria?
			HeapMetadata next;
			next.isFree = true;
			next.prevAlloc = inicio_actual;
			next.nextAlloc = 0; //TODO: NULL?
			memcpy(ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc), &next, SIZE_METADATA);
		}

		return inicio_actual; //retorno la direccion logica
	}
}

void* ubicacion_nuevo_segmento(int num_marco, int dir_log){
	return (ram.memoria + num_marco * configuracion.TAMANIO_PAGINAS + dir_log % configuracion.TAMANIO_PAGINAS);
	     // memoria              inicio                                      desplazamiento
}


int num_pagina_a_traer(uint32_t inicio){ //inicio del nuevo metadata
	int contador = 0;
	while (inicio / configuracion.TAMANIO_PAGINAS > 1){
		inicio -= configuracion.TAMANIO_PAGINAS;
		contador++;
	}
	return contador;
}

HeapMetadata* primer_segmento(tab_pags* tabla){
	dir_t direccion_logica;
	direccion_logica.segmento=0;
	direccion_logica.offset=0;

	dir_t direccion_fisica = convertir_a_df(tabla, direccion_logica);
	int offset = offset_memoria(direccion_fisica);

	HeapMetadata* seg = ram.memoria + offset;
	return seg;
}

bool hay_cambio_de_pagina(int direc_actual, uint32_t next_alloc){
	for (int i=direc_actual+9; i < next_alloc; i++) {
		if (i % configuracion.TAMANIO_PAGINAS == 0) {
			return true;
		}
	}
	return false;
}

int cant_cambios_de_pagina(int direc_actual, uint32_t next_alloc){
	int cant = 0;
	for (int i=direc_actual+9; i < next_alloc; i++) {
		if (i % configuracion.TAMANIO_PAGINAS == 0) {
			cant++;
		}
	}
	return cant;
}

void memfree(int dir_log, int pid){
	tab_pags* tabla_paginas = buscar_page_table(pid);
	HeapMetadata* ptr_potencial_segmento = hallar_metadata(dir_log, tabla_paginas);
	int inicio_actual = 0, num_pagina_actual=0;
}

HeapMetadata* hallar_metadata(uint32_t dir_log){
	do
}


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
