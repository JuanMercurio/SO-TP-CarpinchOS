#include "operaciones.h"

#include "../configuracion/config.h"
#include "../esquema/paginacion.h"
#include "../esquema/paginacion.h"

#include <mensajes/mensajes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/utils.h>

#define SIZE_METADATA sizeof(HeapMetadata)

void TEST_agregar_nueva_pagina(tab_pags* tabla, int marco){
    pag_t* pagina = malloc(sizeof(pag_t));
    pagina->marco = marco;
    list_add(tabla->tabla_pag, pagina);
}
int memalloc(tab_pags* tabla, int tamanio){ //quizas al igual que antes, el carpincho se guarda local en el hilo? entonces no habría que pasarlo como parametro

    int pid = tabla->pid;
    tab_pags* tabla_paginas = tabla;

	printf("- Memalloc: quiero reservar %i para el carpincho %i.\n", tamanio, pid);
	if(tamanio==0){
		puts("- Memalloc: no se puede reservar espacio 0. Abortando.");
		return -2;
	}
	puts("- Memalloc: llamo a buscar la pagina del pid");
	if(tabla_paginas == NULL){
		puts("ERROR CARPINCHO INVALIDO");
		return -1;
	}
	int heap_init_return;
    if (tabla_paginas->tabla_pag->head == NULL) { printf("Tengo que iniciar las paginas\n"); heap_init_return = heap_init(tabla); }
	if (heap_init_return == -1) return -1;

	printf("- Memalloc: encontre la pagina del carpincho %i.\n", tabla_paginas->pid);
	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
	printf("- Memalloc: encontre el primer segmento, que tiene como nextAlloc al %i, y que su valor de isFree es %i, es decir ", ptr_potencial_segmento->nextAlloc, ptr_potencial_segmento->isFree);
	printf((ptr_potencial_segmento->isFree) ? "true.\n" : "false.\n");
	int inicio_actual = 0, num_pagina_actual=0;
	//CAMBIADO
//	pag_t* pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
	puts("- Memalloc: entro al while.");
	int numero_magico; //si nextAlloc es 0, entonces esta variable vale la ultima direccion de memoria+1 (por ejemplo 128), sino vale nextAlloc
	int espacio_en_alloc;
	while(1){

		//  ****BUSCO SI ENTRA****
		printf("- Memalloc->While->CUENTA:\n");
		numero_magico = (ptr_potencial_segmento->nextAlloc == LAST_METADATA ? (list_size(tabla_paginas->tabla_pag)*configuracion.TAMANIO_PAGINAS) : ptr_potencial_segmento->nextAlloc);
		espacio_en_alloc = numero_magico - SIZE_METADATA - inicio_actual;
		printf("\t\tsegmento->nextAlloc: %i.\n", numero_magico);
		printf("\t\tsize_metadata: %i.\n", SIZE_METADATA);
		printf("\t\tinicio_actual %i.\n", inicio_actual);
		printf("\t\tRESULTADO: %i.\n", espacio_en_alloc);
		printf("\t\tTAMANIO: %i.\n", tamanio);

		if(ptr_potencial_segmento->isFree){
			if(espacio_en_alloc == tamanio){
				//entra justo
				puts("-Memalloc->While: entra justo.");
				ptr_potencial_segmento->isFree = false;
				printf("-Memalloc->While: cambio isFree a false y devuelvo direccion de inicio %i.\n", inicio_actual+SIZE_METADATA);
				return inicio_actual + SIZE_METADATA; //retorno la direccion logica
			}

			if(espacio_en_alloc >= tamanio+SIZE_METADATA) { //ACA TENDRIA QUE TENER EN CUENTA SI EL METADATA PUEDE DIVIDRSE EN 2 PAGINAS
				//encontre el segmento donde entra
				//ahora debo evaluar si entra justo o si tengo que dividirlo en 2 partes
				printf("\t\tComo isFree es %i y %i >= %i (%i + %i), entro en el if.\n", ptr_potencial_segmento->isFree, espacio_en_alloc, tamanio+SIZE_METADATA, tamanio, SIZE_METADATA);
				printf("- Memalloc->While: el segmento con inicio en %i es valido para alocar. Esta en la pagina %i.\n", inicio_actual, num_pagina_actual);
				printf("- Memalloc->While: SU PUNTERO ES %p.\n", ptr_potencial_segmento);
				//			if(numero_magico - SIZE_METADATA*2 - inicio_actual == tamanio){
				//				//entra justo
				//				puts("- Memalloc->While: entra justo.");
				//				ptr_potencial_segmento->isFree = false;
				//				return inicio_actual; //retorno la direccion logica
				//			}
				puts("- Memalloc->While: tengo que dividir en 2.");
				//tengo que dividir en 2
				printf("- Memalloc->While: ptr_potencial_segmento->isFree es %d\n", ptr_potencial_segmento->isFree);
				ptr_potencial_segmento->isFree = false;
				printf("- Memalloc->While: ptr_potencial_segmento->isFree es %d\n", ptr_potencial_segmento->isFree);
				ptr_potencial_segmento->isFree = false;
				int aux_next_alloc = ptr_potencial_segmento->nextAlloc;
				ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;

				//agrego el heapmetadata elegido actualizado 
				// esta bien inicio actua? 
				memoria_escribir_por_dirlog(tabla, inicio_actual, ptr_potencial_segmento, sizeof(HeapMetadata));

				HeapMetadata new;
				new.prevAlloc = inicio_actual;
				new.nextAlloc = aux_next_alloc;
				new.isFree = true;

				HeapMetadata* next;
				//si el nuevo metadata esta en otra pagina (CAMBIADO, SOLO QUEDA POR LOS PUTS)
				if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
					printf("HAY CAMBIO DE PAGINA ENTRE %i Y %i.\n", inicio_actual, ptr_potencial_segmento->nextAlloc);
					puts("- Memalloc->While: el metadata esta en otra pagina.");
				} else {
					printf("- Memalloc->While: NO HAY CAMBIO DE PAGINA ENTRE %i Y %i.\n", inicio_actual, ptr_potencial_segmento->nextAlloc);
					puts("- Memalloc->While: el metadata NO esta en otra pagina.");
				}

				if(new.nextAlloc!=LAST_METADATA) {
					puts("- Memalloc->While: existe metadata next");
					//CAMBIADO
					//						if(hay_cambio_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc)) { //caso borde, el segmento a cambiarle el prevalloc tambien esta en otra pagina
					//							pag_t* pagina_next = list_get(tabla_paginas->tabla_pag, num_pag_a_traer + cant_cambios_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc));
					//							//: la cargo en memoria
					//							next = ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc);
					//						}
					//						else{
					//							next = ubicacion_nuevo_segmento(pagina->marco, new.nextAlloc);
					//						}
					next = memoria_leer_por_dirlog(tabla_paginas, new.nextAlloc, SIZE_METADATA);
					next->prevAlloc = ptr_potencial_segmento->nextAlloc;
				}
				else puts("- Memalloc->While: no existe metadata next");



				//QUIZAS ACA TENDRIA QUE EVALUAR POR METADATA DIVIDIDO (ya no jaja!)
				//CAMBIADO
//				memcpy(ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc), &new, SIZE_METADATA);
				memoria_escribir_por_dirlog(tabla_paginas, ptr_potencial_segmento->nextAlloc, &new, SIZE_METADATA);
//				printf("- Memalloc->While: cargue el nuevo segmento en la direccion logica %i, puntero %p.\n", ptr_potencial_segmento->nextAlloc, ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc));

				printf("- Memalloc->While: RETORNO DIR_LOG DE GUARDADO: %i. METADATA ARRANCA EN %i.\n", inicio_actual+SIZE_METADATA, inicio_actual);
				return inicio_actual + SIZE_METADATA;
			}
		}



		//  ****BUSCO SI ES EL ULTIMO METADATA****

		if(ptr_potencial_segmento->nextAlloc == LAST_METADATA){
			puts("- Memalloc->While: llegue al final y no encontre nada");
			if(espacio_en_alloc >= tamanio){
				puts("- Memalloc->While: la razon por la que no entra es que no le entra el nuevo metadata.\n"
						"Esto significa que nunca le va a entrar el nuevo metadata, por lo que declino la operación sin agregar pagina nueva.");
				return -1;
			}

			printf("- Memalloc->Pedir: estoy en la pagina numero %i.\n", num_pagina_actual);
			printf("- Memalloc->Pedir: mi inicio actual es %i.\n", inicio_actual);

			puts("- Memalloc->Pedir: AGREGO PAG NUEVA.");
			//TEST_agregar_nueva_pagina(tabla_paginas, 3);
			//if(pedir_memoria_a_swap(tamanio+SIZE_METADATA*2) == false){ //TODO: placeholder
			if(false){
				return -1; //mate_noseque_fault
			}
			else{
				if(ptr_potencial_segmento->isFree) {
					puts("- Memalloc->Pedir->Free: el ultimo alloc esta libre. Lo expando.");
					if(false){ //TODO: testear.
						ptr_potencial_segmento->isFree=false;
						return inicio_actual+SIZE_METADATA;
					}
					puts("- Memalloc->Pedir->Free: tengo que dividir en 2.");
					//tengo que dividir en 2                   			DUDA: que pasa si el lugar que queda es menor al tamaño de un metadata?
					printf("ESPACIO_EN_ALLOC: %i", espacio_en_alloc);
					ptr_potencial_segmento->isFree = false;
					ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;

					memoria_escribir_por_dirlog(tabla, inicio_actual, ptr_potencial_segmento, sizeof(HeapMetadata));

					HeapMetadata new;
					new.prevAlloc = inicio_actual;
					new.nextAlloc = LAST_METADATA;
					new.isFree = true;


					puts("- Memalloc->Pedir->Free: el metadata esta en otra pagina, obviamente.");
					//CAMBIADO
//					puts("- Memalloc->Pedir->Free->CUENTA:");
//					printf("\t\t\tnum_pagina_actual: %i.\n", num_pagina_actual);
//					printf("\t\t\tinicio_actual: %i.\n", inicio_actual);
//					printf("\t\t\tnextAlloc: %i.\n", ptr_potencial_segmento->nextAlloc);
//					printf("\t\t\tcant_cambios_de_pagina: %i.\n", cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc));
//					int num_pag_a_traer = num_pagina_actual + cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc); //deberia ser siempre la ultima, no?
//					printf("- Memalloc->Pedir->Free: debo traer la pagina %i.\n", num_pag_a_traer);
//					pagina = list_get(tabla_paginas->tabla_pag, num_pag_a_traer);
//					puts("- Memalloc->Pedir->Free: traje la pagina");
//
//					//: la cargo en memoria
//
//					memcpy(ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc), &new, SIZE_METADATA);
					int new_pags = paginas_a_agregar(ptr_potencial_segmento->nextAlloc, tabla);
					if (new_pags == -1) return -1;
					new_pags = paginas_agregar(new_pags, tabla);
					memoria_escribir_por_dirlog(tabla_paginas, ptr_potencial_segmento->nextAlloc, &new, SIZE_METADATA);
//					printf("- Memalloc->Pedir->Free: cargue el nuevo segmento en la direccion logica %i, puntero %p.\n", ptr_potencial_segmento->nextAlloc, ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc));

					printf("- Memalloc->Pedir->Free: RETORNO DIR_LOG DE GUARDADO: %i. METADATA ARRANCA EN %i.\n", inicio_actual+SIZE_METADATA, inicio_actual);
					return inicio_actual + SIZE_METADATA;
				}
				else{
					puts("- Memalloc->Pedir->NoFree: el ultimo alloc esta ocupado. Genero nuevo metadata al principio de la pagina.");
					//CAMBIADO
					int num_primera_pagina_nueva = num_pagina_actual+1; //(este no cambiado)
//					pag_t* primera_pagina_nueva = list_get(tabla_paginas->tabla_pag, num_primera_pagina_nueva);
//					printf("- Memalloc->Pedir->NoFree: agarre la pagina %i.\n", num_primera_pagina_nueva);

					int inicio_anterior = inicio_actual;
					inicio_actual = num_primera_pagina_nueva*configuracion.TAMANIO_PAGINAS;
					ptr_potencial_segmento->nextAlloc = inicio_actual;
					printf("- Memalloc->Pedir->NoFree: cambie el nextAlloc del segmento anterior a %i.\n", ptr_potencial_segmento->nextAlloc);

					HeapMetadata new;
					new.isFree = false;
					new.prevAlloc = inicio_anterior;
					dir_t dir_new;
					dir_new.offset=0;
					dir_new.PAGINA=num_primera_pagina_nueva;

					if((tamanio + SIZE_METADATA) % configuracion.TAMANIO_PAGINAS == 0){//entra justo
						puts("- Memalloc->Pedir->NoFree: el nuevo metadata entra justo.");

						new.nextAlloc = LAST_METADATA;
						printf("- Memalloc->Pedir->NoFree: cree el nuevo metadata, con isFree = %s; prev = %i, next = %i.\n", (new.isFree?"true":"false"), new.prevAlloc, new.nextAlloc);

						//CAMBIADO
//						memcpy(ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0), &new, SIZE_METADATA); //cargo la nueva metadata
//						printf("- Memalloc->Pedir->NoFree: cargue el nuevo metadata en la direccion %i, puntero %p.\n", ptr_potencial_segmento->nextAlloc, ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0));
					}
					else{
						puts("- Memalloc->Pedir->NoFree: tengo que dividir en 2.");

						new.nextAlloc = inicio_actual + SIZE_METADATA + tamanio;
						printf("- Memalloc->Pedir->NoFree: cree el nuevo metadata, con isFree = %s; prev = %i, next = %i.\n", (new.isFree?"true":"false"), new.prevAlloc, new.nextAlloc);
						//CAMBIADO
//						memcpy(ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0), &new, SIZE_METADATA);
						//CAMBIADO
//						int num_pagina_next = list_size(tabla_paginas->tabla_pag)-1;
//						printf("- Memalloc->Pedir->NoFree: el metadata next esta en la pagina %i.\n", num_pagina_next);
//						pag_t* pagina_next;
//						if(num_primera_pagina_nueva != num_pagina_next){ //si el next queda en otra pagina
//							pagina_next = list_get(tabla_paginas->tabla_pag, num_pagina_next);
//							puts("- Memalloc->Pedir->NoFree: agarro la pagina");
//							//: la traigo a memoria
//						} else pagina_next = primera_pagina_nueva;

						HeapMetadata next;
						next.isFree = true;
						next.prevAlloc = inicio_actual;
						next.nextAlloc = LAST_METADATA;
						printf("- Memalloc->Pedir->NoFree: cree el next metadata, con isFree = %s; prev = %i, next = %i.\n", (next.isFree?"true":"false"), next.prevAlloc, next.nextAlloc);
						//CAMBIADO
//						memcpy(ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc), &next, SIZE_METADATA);
						memoria_escribir_por_dirlog(tabla_paginas, new.nextAlloc, &next, SIZE_METADATA);
//						printf("- Memalloc->Pedir->NoFree: cargue el next metadata en la direccion %i, puntero %p.\n", new.nextAlloc, ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc));
					}

					memoria_escribir(tabla_paginas, dir_new, &new, SIZE_METADATA);
					printf("- Memalloc->Pedir->NoFree: RETORNO DIRECCION LOGICA %i.\n", ptr_potencial_segmento->nextAlloc+SIZE_METADATA);
					return ptr_potencial_segmento->nextAlloc+SIZE_METADATA; //retorno la direccion logica
				}
			}
		}




		puts("- Memalloc->While: este segmento no es valido para alocar. Pasando al proximo segmento.");
		//Si no entre a los if anteriores, significa que no encontre un hueco todavia. Tengo que pasar al siguiente alloc.

		//CAMBIADO
//		if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
//			//El siguiente metadata esta en otra pagina
//			puts("- Memalloc->While: el siguiente metadata esta en otra pagina.");
//			//Obtengo dir fisica de inicio de pagina nueva, luego el metadata esta en [direccion.segmento + configuracion.TAMANIO_PAGINA - inicio_actual]
//			num_pagina_actual += cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc);
//			printf("- Memalloc->While: tengo que buscar la pagina %i.\n", num_pagina_actual);
//			pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
//			//: la cargo en memoria
//		}

		inicio_actual = ptr_potencial_segmento->nextAlloc;
		printf("- Memalloc->While: el nuevo inicio sera %i.\n", inicio_actual);
		//CAMBIADO
//		ptr_potencial_segmento = ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc); //: REVISAR
		ptr_potencial_segmento = memoria_leer_por_dirlog(tabla_paginas, ptr_potencial_segmento->nextAlloc, SIZE_METADATA);
		printf("- Memalloc->While: el nuevo puntero a revisar es el %p.\n", ptr_potencial_segmento);
	}

	return 0;
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


	return (HeapMetadata*) memoria_leer(tabla, direccion_logica, SIZE_METADATA);
}

bool hay_cambio_de_pagina(int direc_actual, uint32_t next_alloc){
	for (int i=direc_actual+1; i <= next_alloc; i++) {
		if (i % configuracion.TAMANIO_PAGINAS == 0) {
			return true;
		}
	}
	return false;
}

int cant_cambios_de_pagina(int direc_actual, uint32_t next_alloc){
	int cant = 0;
	if(direc_actual < next_alloc){
		for (int i=direc_actual; i <= next_alloc; i++) {
			if (i % configuracion.TAMANIO_PAGINAS == 0) {
				cant++;
			}
		}
	} else {
		for (int i=next_alloc; i <= direc_actual; i++) {
			if (i % configuracion.TAMANIO_PAGINAS == 0) {
				cant--;
			}
		}
	}

	return cant;
}

int memfree(tab_pags* tabla, int dir_log){

    tab_pags* tabla_paginas = tabla;

	printf("- Memfree: quiero liberar la direccion logica %i.\n", dir_log);

	if(dir_log >= configuracion.TAMANIO){
		puts("- Memfree->ERROR: direccion logica imposible.");
		return -5;
	}
	puts("Memfree: busco la page table.");

	//CAMBIADO
//	int num_pag;
//	HeapMetadata* ptr_segmento = hallar_metadata(dir_log, tabla_paginas, &num_pag);
	// HeapMetadata* ptr_segmento = hallar_metadata(dir_log, tabla_paginas);
	HeapMetadata* ptr_segmento = memoria_leer_por_dirlog(tabla, dir_log - sizeof(HeapMetadata), sizeof(HeapMetadata));

	if(ptr_segmento == NULL){
		puts("- Memfree->ERROR: direccion logica invalida");
		return -5; //
	}
//	printf("- Memfree: la pagina del segmento es la %i.\n", num_pag);
	printf("- Memfree: isFree = %d\n", ptr_segmento->isFree);
	if(ptr_segmento->isFree == 1){
		puts("- Memfree: el segmento ya esta libre.");
		return -5;
	}

//	pag_t* pagina_actual = list_get(tabla_paginas->tabla_pag, num_pag);
	ptr_segmento->isFree = true;
	puts("- Memfree: cambio el isFree a true.");

	int inicio_actual = dir_log;
	HeapMetadata *ptr_derecha, *ptr_izquierda;
//	int dir_nextAlloc = ptr_segmento->nextAlloc;

	puts("- Memfree->Derecha: reviso a derecha.");
	if(ptr_segmento->nextAlloc != LAST_METADATA){
		//CAMBIADO
//		if(hay_cambio_de_pagina(inicio_actual, ptr_segmento->nextAlloc)){
//			puts("- Memfree->Derecha: el derecho esta en otra pagina.");
//			pag_t* pagina_derecha = list_get(tabla_paginas->tabla_pag, num_pag + cant_cambios_de_pagina(inicio_actual, ptr_segmento->nextAlloc));
//			//: la cargo en memoria
//			ptr_derecha = ubicacion_nuevo_segmento(pagina_derecha->marco, ptr_segmento->nextAlloc);
//		}else{
//			ptr_derecha = ubicacion_nuevo_segmento(pagina_actual->marco, ptr_segmento->nextAlloc); //: REVISAR
//		}
//		printf("- Memfree->Derecha: el puntero a derecha esta en el puntero %p.\n", ptr_derecha);
		ptr_derecha = memoria_leer_por_dirlog(tabla_paginas, ptr_segmento->nextAlloc, SIZE_METADATA);
		if(ptr_derecha->isFree){
			puts("- Memfree->Derecha: el puntero a derecha esta libre. Procedo a liberarlo.");
			ptr_segmento->nextAlloc = ptr_derecha->nextAlloc;
//			dir_nextAlloc = ptr_segmento->nextAlloc;
		} else puts("- Memfree->Derecha: el puntero a derecha esta ocupado.");
	} else puts("- Memfree->Derecha: no existe puntero a derecha.");


	//
	puts("- Memfree->Izquierda: reviso a izquierda.");
	if(ptr_segmento->prevAlloc != FIRST_METADATA){
		//CAMBIADO
//		if(hay_cambio_de_pagina(ptr_segmento->prevAlloc, inicio_actual)){
//			puts("- Memfree->Izquierda: el izquierdo esta en otra pagina.");
//			pag_t* pagina_izquierda = list_get(tabla_paginas->tabla_pag, num_pag + cant_cambios_de_pagina(inicio_actual, ptr_segmento->prevAlloc)); //: checkear porque cambie - por +
//			//: la cargo en memoria
//			ptr_izquierda = ubicacion_nuevo_segmento(pagina_izquierda->marco, ptr_segmento->prevAlloc);
//		}else{
//			ptr_izquierda = ubicacion_nuevo_segmento(pagina_actual->marco, ptr_segmento->prevAlloc); //: REVISAR
//		}
//		printf("- Memfree->Izquierda: el puntero a izquierda esta en el puntero %p.\n", ptr_izquierda);
		ptr_izquierda = memoria_leer_por_dirlog(tabla_paginas, ptr_segmento->prevAlloc, SIZE_METADATA);
		if(ptr_izquierda->isFree){
//			num_pag += cant_cambios_de_pagina(inicio_actual, ptr_segmento->nextAlloc);
			puts("- Memfree->Izquierda: el puntero a izquierda esta libre. Procedo a eliminarlo.");
			ptr_izquierda->nextAlloc = ptr_segmento->nextAlloc;
			inicio_actual = ptr_segmento->prevAlloc;
			ptr_segmento = ptr_izquierda;
		} else puts("- Memfree->Izquierda: el puntero a izquierda esta ocupado.");
	} else puts("- Memfree->Izquierda: no existe puntero a izquierda.");


	puts("- Memfree->Liberar: reviso si hay que liberar paginas.");
	if(ptr_segmento->nextAlloc == LAST_METADATA){
		int espacio_en_alloc = list_size(tabla_paginas->tabla_pag)*configuracion.TAMANIO_PAGINAS - SIZE_METADATA - inicio_actual;
		if(espacio_en_alloc >= configuracion.TAMANIO_PAGINAS){
			//tengo que liberar al menos una pagina
			int cant_pags_a_liberar = espacio_en_alloc/configuracion.TAMANIO_PAGINAS;
			ptr_segmento->nextAlloc -= cant_pags_a_liberar*configuracion.TAMANIO_PAGINAS;
			for(int i=1; i<=cant_pags_a_liberar; i++){
				list_remove(tabla_paginas->tabla_pag, list_size(tabla_paginas->tabla_pag)-i); //TODO: hace falta algo mas?
			}
		}
	}

	return 1;
}


bool pedir_memoria_a_swap(int tamanio){
	return true;
}

//CAMBIADO
HeapMetadata* hallar_metadata(uint32_t dir_log, tab_pags* tabla){//, int* num_pag){
	dir_log -= sizeof(HeapMetadata);
	HeapMetadata* potencial_segmento = primer_segmento(tabla);
	uint32_t inicio_actual = 0;
	int num_pagina_actual = 0;
	
	// HeapMetadata* potencial_segmento = memoria_leer_por_dirlog(tabla, dir_log, sizeof(HeapMetadata));
	// uint32_t inicio_actual = 0;
	// int num_pagina_actual = 0;

	pag_t* pagina = list_get(tabla->tabla_pag, num_pagina_actual);
	while(inicio_actual != dir_log){
		printf("Memfree->Hallar Metadata: el inicio actual %i es distinto al que busco %i.\n", inicio_actual, dir_log);
		if(potencial_segmento->nextAlloc == LAST_METADATA) return NULL;

		if(hay_cambio_de_pagina(inicio_actual, potencial_segmento->nextAlloc)){
			num_pagina_actual += cant_cambios_de_pagina(inicio_actual, potencial_segmento->nextAlloc);
			printf("Memfree->Hallar Metadata: voy a buscar al proximo en la pagina %i.\n", num_pagina_actual);
			pagina = list_get(tabla->tabla_pag, num_pagina_actual);
			//TODO: la cargo en memoria
		}
		inicio_actual = potencial_segmento->nextAlloc;
		potencial_segmento = ubicacion_nuevo_segmento(pagina->marco, potencial_segmento->nextAlloc);
	}
	printf("Memfree->Hallar Metadata: encontre la metadata en la pagina %i.\n", num_pagina_actual);
	//*num_pag = num_pagina_actual;
	return potencial_segmento;
}

void TEST_report_metadatas(int pid){
	puts("--COMIENZO REPORTE METADATA--");
	tab_pags* tabla_paginas = buscar_page_table(pid);
	if(tabla_paginas == NULL){
		puts("ERROR CARPINCHO INVALIDO");
		return;
	}
	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
	int inicio_actual = 0, num_pagina_actual=0;
	pag_t* pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
	int numero_magico, espacio_en_alloc;
	int num_meta = 0;
	printf("\tPAGINA 0 (MARCO %i):\n", pagina->marco);
	while(1){

		numero_magico = (ptr_potencial_segmento->nextAlloc == LAST_METADATA ? (list_size(tabla_paginas->tabla_pag)*configuracion.TAMANIO_PAGINAS) : ptr_potencial_segmento->nextAlloc);

		espacio_en_alloc = numero_magico - SIZE_METADATA - inicio_actual;
		printf("\tMetadata %i:\tinicio\t\tprev\t\tnext\t\ttamanio\t\tisFree\n", num_meta++);
		printf("\t\t\t%i\t\t%i\t\t%i\t\t%i\t\t", inicio_actual, ptr_potencial_segmento->prevAlloc, ptr_potencial_segmento->nextAlloc, espacio_en_alloc);
		printf((ptr_potencial_segmento->isFree) ? "true\n" : "false\n");

		if(ptr_potencial_segmento->nextAlloc == LAST_METADATA){
			puts("--FIN REPORTE METADATA--\n\n");
			return;
		}

		if(ptr_potencial_segmento->nextAlloc==inicio_actual){
			puts("--ERROR DE LOOP, ABORTANDO!!--");
			return;
		}

		if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
			num_pagina_actual += cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc);
			pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
			printf("\tPAGINA %i (MARCO %i):\n", num_pagina_actual, pagina->marco);

		}
		inicio_actual = ptr_potencial_segmento->nextAlloc;
		ptr_potencial_segmento = ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc);
	}
}


// int memalloc(tab_pags *tabla, int tamanio){return 0;}
// int memfree(tab_pags *tabla, int dl){return 0;}

void* memread(tab_pags* tabla, int dir_log, int tamanio){
    dir_t dl = traducir_dir_log(dir_log);
	// dir_t dl = decimal_a_dl(dir_log);

    // if(!alloc_valido(dl, tabla, tamanio)) return NULL; 
    return memoria_leer(tabla, dl, tamanio);
}

int memwrite(tab_pags* tabla, int dir_log, void* contenido, int tamanio){
    dir_t dl = traducir_dir_log(dir_log);
	// dir_t dl = decimal_a_dl(dir_log);

    // if(!alloc_valido(dl, tabla, tamanio)) return MATE_WRITE_FAULT;

    return memoria_escribir(tabla, dl, contenido, tamanio);
}

void* memoria_leer(tab_pags* tabla, dir_t dl, int tamanio){

    if(!read_verify_size(tabla, dl, tamanio)) return NULL; 

    int leido = 0;
    int bytes_remaining = tamanio;
    int page_remaining_space = configuracion.TAMANIO_PAGINAS - dl.offset;

    void* buffer = malloc(tamanio);

    while(bytes_remaining > 0)
    {
        int marco = nro_marco(dl.PAGINA, tabla);
        dir_t df = { marco, dl.offset };
        int leer = min_get(bytes_remaining, page_remaining_space);

        memcpy(buffer + leido, ram.memoria + offset_memoria(df), leer);
		pag_t* pagina = list_get(tabla->tabla_pag, dl.PAGINA);

        page_use(tabla->pid, marco, pagina, dl.PAGINA, READ);

        page_remaining_space = configuracion.TAMANIO_PAGINAS;
        bytes_remaining -= leer;
        leido += leer;
        dl.offset = 0;
        dl.PAGINA++;
    }

    return buffer;
}

int memoria_escribir(tab_pags* tabla, dir_t dl, void* contenido, int tamanio){
    if(!read_verify_size(tabla, dl, tamanio)) return MATE_WRITE_FAULT;    

    int written = 0;
    int bytes_remaining = tamanio;
    int bytes_remaining_space = configuracion.TAMANIO_PAGINAS - dl.offset;
	printf("ESCRIBIR: quiero escribir en PAG: %d PID: %d \n", dl.PAGINA, tabla->pid);
    while(bytes_remaining > 0)
    {
        int marco = nro_marco(dl.PAGINA, tabla);
		if (marco < 0) {
			fprintf(stderr, "\n Error con el nro de marco obtenido \n\n");
		}

        dir_t df = { marco, dl.offset };
        int bytes_to_write = min_get(bytes_remaining, bytes_remaining_space);

        memcpy(ram.memoria + offset_memoria(df), contenido + written, bytes_to_write);
		pag_t* pagina = list_get(tabla->tabla_pag, dl.PAGINA);

        page_use(tabla->pid, marco, pagina, dl.PAGINA, WRITE);

        bytes_remaining_space = configuracion.TAMANIO_PAGINAS;
        bytes_remaining -= bytes_to_write;
        written += bytes_to_write;
        dl.offset = 0;
        dl.PAGINA++;
    }
    return 0;
}

bool alloc_valido(dir_t dl, tab_pags* t, int tamanio){
    dir_t heap_location = heap_get_location(dl, t);
    HeapMetadata* data = (HeapMetadata*) memoria_leer(t, heap_location, sizeof(HeapMetadata));

    return data->isFree == false && heap_lectura_valida(data, dl, tamanio);
}

bool heap_lectura_valida(HeapMetadata* data, dir_t dl, int tamanio){
    int tamanio_posible = data->nextAlloc - configuracion.TAMANIO_PAGINAS*dl.PAGINA +dl.offset;
    return tamanio_posible >= tamanio;
}

dir_t heap_get_location(dir_t dl, tab_pags* t){
    if(dl.offset - sizeof(HeapMetadata) < 0)
    {
        dl.PAGINA--;
        dl.offset = configuracion.TAMANIO_PAGINAS + (dl.offset - sizeof(HeapMetadata));
    }

    else
    {
       dl.offset -= sizeof(HeapMetadata);
    }

    return dl; 
}


bool read_verify_size(tab_pags* t, dir_t dl, int tamanio){
    int pages_count = list_size(t->tabla_pag);
    int readable_bytes = read_get_readable_bytes(dl, pages_count);
    return readable_bytes >= tamanio;
}

int read_get_readable_bytes(dir_t dl, int count){ 
    int readable_bytes = count*configuracion.TAMANIO_PAGINAS;
    readable_bytes -= dl.PAGINA*configuracion.TAMANIO_PAGINAS - dl.offset;
    return readable_bytes;
}



void* memoria_leer_por_dirlog(tab_pags* tabla, int dl, int tamanio){
	// dir_t dir;
	// dir.PAGINA = dl/configuracion.TAMANIO_PAGINAS;
	// dir.offset = dl%configuracion.TAMANIO_PAGINAS;
	int bin_dl = decimal_a_binario(dl);
	dir_t dir = traducir_dir_log(bin_dl);
	return memoria_leer(tabla, dir, tamanio);
}

int memoria_escribir_por_dirlog(tab_pags* tabla, int dl, void* contenido, int tamanio){
	// dir_t dir;
	// dir.PAGINA = dl/configuracion.TAMANIO_PAGINAS;
	// dir.offset = dl%configuracion.TAMANIO_PAGINAS;
	int bin = decimal_a_binario(dl);
	dir_t dir = traducir_dir_log(bin);
	return memoria_escribir(tabla, dir, contenido, tamanio);
}


int heap_init(tab_pags* tabla)
{
	int estado_swap = swap_pedir_paginas(tabla, 1);  //que pasa si las paginas son de 8?
	if (estado_swap == -1) return -1;

    HeapMetadata* data = malloc(SIZE_METADATA);
    data->isFree = true;
    data->nextAlloc = LAST_METADATA;
    data->prevAlloc = FIRST_METADATA;

    dir_t dl = {0, 0};

	pagina_iniciar(tabla);
    memoria_escribir(tabla, dl, data, SIZE_METADATA);
    
}


int paginas_a_agregar(int dl, tab_pags* tabla)
{
	int paginas = list_size(tabla->tabla_pag);
	int victima = (dl+sizeof(HeapMetadata)) / configuracion.TAMANIO_PAGINAS;

	int nuevas = victima - paginas + 1;
	//verficiar con swamp y asignacion 
	int estado_swap = swap_pedir_paginas(tabla, nuevas);
	if (estado_swap == -1) return -1;

	return nuevas;
	}

int swap_pedir_paginas(tab_pags* tabla, int c_paginas)
{
	int paginas_totales = list_size(tabla->tabla_pag);
	int pagina = paginas_totales;
	int estado;

	for (int i=0; i < c_paginas; ++i) {
		enviar_int(swap, SOLICITUD_PAGINA);
		enviar_int(swap, tabla->pid);
		enviar_int(swap, pagina);

		estado = recibir_int(swap);
		if (estado == -1) break;

		pagina++;
	}

	return estado;
}

int paginas_agregar(int new_pags, tab_pags* tabla)
{
	for (int i=0; i < new_pags; i++) {
		pagina_iniciar(tabla);
	}

	return 0;
}