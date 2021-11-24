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

// int memalloc(int tamanio, int pid){ //quizas al igual que antes, el carpincho se guarda local en el hilo? entonces no habría que pasarlo como parametro
// 	printf("- Memalloc: quiero reservar %i para el carpincho %i.\n", tamanio, pid);
// 	puts("- Memalloc: llamo a buscar la pagina del pid");
// 	tab_pags* tabla_paginas = buscar_page_table(pid);
// 	if(tabla_paginas == NULL){
// 		puts("ERROR CARPINCHO INVALIDO");
// 		return -1;
// 	}
// 	printf("- Memalloc: encontre la pagina del carpincho %i.\n", tabla_paginas->pid);
// 	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
// 	printf("- Memalloc: encontre el primer segmento, que tiene como nextAlloc al %i, y que su valor de isFree es %i, es decir ", ptr_potencial_segmento->nextAlloc, ptr_potencial_segmento->isFree);
// 	printf((ptr_potencial_segmento->isFree) ? "true.\n" : "false.\n");
// 	int inicio_actual = 0, num_pagina_actual=0;
// 	pag_t* pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
// 	puts("- Memalloc: entro al while.");
// 	int numero_magico; //si nextAlloc es 0, entonces esta variable vale la ultima direccion de memoria, sino vale nextAlloc
// 	int espacio_en_alloc;
// 	while(1){

// 		//  ****BUSCO SI ENTRA****
// 		printf("- Memalloc->While->CUENTA:\n");
// 		numero_magico = (ptr_potencial_segmento->nextAlloc == LAST_METADATA ? (list_size(tabla_paginas->tabla_pag)*configuracion.TAMANIO_PAGINAS -1) : ptr_potencial_segmento->nextAlloc);
// 		espacio_en_alloc = numero_magico - SIZE_METADATA - inicio_actual +1;
// 		printf("\t\tsegmento->nextAlloc: %i.\n", numero_magico);
// 		printf("\t\tsize_metadata: %i.\n", SIZE_METADATA);
// 		printf("\t\tinicio_actual %i.\n", inicio_actual);
// 		printf("\t\tRESULTADO: %i.\n", espacio_en_alloc);
// 		printf("\t\tTAMANIO: %i.\n", tamanio);

// 		if(ptr_potencial_segmento->isFree){
// 			if(espacio_en_alloc == tamanio){
// 				//entra justo
// 				puts("-Memalloc->While: entra justo.");
// 				ptr_potencial_segmento->isFree = false;
// 				printf("-Memalloc->While: cambio isFree a false y devuelvo direccion de inicio %i.\n", inicio_actual+SIZE_METADATA);
// 				return inicio_actual + SIZE_METADATA; //retorno la direccion logica
// 			}

// 			if(espacio_en_alloc >= tamanio+SIZE_METADATA) {
// 				//encontre el segmento donde entra
// 				//ahora debo evaluar si entra justo o si tengo que dividirlo en 2 partes
// 				printf("\t\tComo isFree es %i y %i >= %i (%i + %i), entro en el if.\n", ptr_potencial_segmento->isFree, espacio_en_alloc, tamanio+SIZE_METADATA, tamanio, SIZE_METADATA);
// 				printf("- Memalloc->While: el segmento con inicio en %i es valido para alocar. Esta en la pagina %i.\n", inicio_actual, num_pagina_actual);
// 				printf("- Memalloc->While: SU PUNTERO ES %p.\n", ptr_potencial_segmento);
// 				//			if(numero_magico - SIZE_METADATA*2 - inicio_actual == tamanio){
// 				//				//entra justo
// 				//				puts("- Memalloc->While: entra justo.");
// 				//				ptr_potencial_segmento->isFree = false;
// 				//				return inicio_actual; //retorno la direccion logica
// 				//			}
// 				puts("- Memalloc->While: tengo que dividir en 2.");
// 				//tengo que dividir en 2                   			DUDA: que pasa si el lugar que queda es menor al tamaño de un metadata?
// 				ptr_potencial_segmento->isFree = false;
// 				int aux_next_alloc = ptr_potencial_segmento->nextAlloc;
// 				ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;

// 				HeapMetadata new;
// 				new.prevAlloc = inicio_actual;
// 				new.nextAlloc = aux_next_alloc;
// 				new.isFree = true;

// 				HeapMetadata* next;
// 				//si el nuevo metadata esta en otra pagina
// 				if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
// 					puts("- Memalloc->While: el metadata esta en otra pagina.");
// 					int num_pag_a_traer = num_pagina_actual + cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc);
// 					printf("- Memalloc->While: debo traer la pagina %i.\n", num_pag_a_traer);
// 					pagina = list_get(tabla_paginas->tabla_pag, num_pag_a_traer);

// 					//TODO: la cargo en memoria

// 					if(new.nextAlloc!=LAST_METADATA) {
// 						puts("- Memalloc->While: existe metadata next");
// 						if(hay_cambio_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc)) { //caso borde, el segmento a cambiarle el prevalloc tambien esta en otra pagina
// 							pag_t* pagina_next = list_get(tabla_paginas->tabla_pag, num_pag_a_traer + cant_cambios_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc));
// 							//TODO: la cargo en memoria
// 							next = ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc);
// 						}
// 						else{
// 							next = ubicacion_nuevo_segmento(pagina->marco, new.nextAlloc);
// 						}
// 						next->prevAlloc = ptr_potencial_segmento->nextAlloc;
// 					}
// 					else puts("- Memalloc->While: no existe metadata next");

// 				}
// 				else{
// 					puts("- Memalloc->While: el metadata NO esta en otra pagina.");
// 					if(new.nextAlloc!=0) {
// 						if(hay_cambio_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc)){
// 							puts("- Memalloc->While: el metadata next (para cambiarle el prevAlloc) esta en otra pagina.");
// 							pag_t* pagina_next = list_get(tabla_paginas->tabla_pag, num_pagina_actual + cant_cambios_de_pagina(ptr_potencial_segmento->nextAlloc, new.nextAlloc));
// 							//TODO: la cargo en memoria
// 							next = ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc);
// 						}
// 						else{
// 							puts("- Memalloc->While: el metadata next (para cambiarle el prevAlloc) NO esta en orta pagina.");
// 							next = ubicacion_nuevo_segmento(pagina->marco, new.nextAlloc);
// 						}
// 						next->prevAlloc = ptr_potencial_segmento->nextAlloc;
// 						printf("- Memalloc->While: cambie el prevAlloc del segmento next, ahora vale %i.\n", next->prevAlloc);
// 					}
// 					else puts("- Memalloc->While: no existe metadata next");
// 				}

// 				memcpy(ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc), &new, SIZE_METADATA);
// 				printf("- Memalloc->While: cargue el nuevo segmento en la direccion logica %i, puntero %p.\n", ptr_potencial_segmento->nextAlloc, ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc));

// 				printf("- Memalloc->While: RETORNO DIR_LOG DE GUARDADO: %i. METADATA ARRANCA EN %i.\n", inicio_actual+SIZE_METADATA, inicio_actual);
// 				return inicio_actual + SIZE_METADATA;
// 			}
// 		}



// 		//  ****BUSCO SI ES EL ULTIMO METADATA****

// 		if(ptr_potencial_segmento->nextAlloc == LAST_METADATA){
// 			puts("- Memalloc->While: llegue al final y no encontre nada");

// 			printf("- Memalloc->Pedir: estoy en la pagina numero %i.\n", num_pagina_actual);
// 			printf("- Memalloc->Pedir: mi inicio actual es %i.\n", inicio_actual);

// 			puts("- Memalloc->Pedir: AGREGO PAG NUEVA.");
// 			TEST_agregar_nueva_pagina(tabla_paginas, 1);
// 			//if(pedir_memoria_a_swap(tamanio+SIZE_METADATA*2) == false){ //TODO: placeholder
// 			if(false){
// 				return -1; //mate_noseque_fault
// 			}
// 			else{ //TODO: duda, todas las paginas que pido nuevas se encuentran en memoria?
// 				//TODO: TESTEAR
// 				if(ptr_potencial_segmento->isFree) {
// 					puts("- Memalloc->Pedir->Free: el ultimo alloc esta libre. Lo expando.");
// 					if(false){ //TODO: entra justo

// 					}
// 					puts("- Memalloc->Pedir->Free: tengo que dividir en 2.");
// 					//tengo que dividir en 2                   			DUDA: que pasa si el lugar que queda es menor al tamaño de un metadata?
// 					ptr_potencial_segmento->isFree = false;
// 					ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;

// 					HeapMetadata new;
// 					new.prevAlloc = inicio_actual;
// 					new.nextAlloc = LAST_METADATA;
// 					new.isFree = true;


// 					puts("- Memalloc->Pedir->Free: el metadata esta en otra pagina, obviamente.");
// 					puts("- Memalloc->Pedir->Free->CUENTA:");
// 					printf("\t\t\tnum_pagina_actual: %i.\n", num_pagina_actual);
// 					printf("\t\t\tinicio_actual: %i.\n", inicio_actual);
// 					printf("\t\t\tnextAlloc: %i.\n", ptr_potencial_segmento->nextAlloc);
// 					printf("\t\t\tcant_cambios_de_pagina: %i.\n", cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc));
// 					int num_pag_a_traer = num_pagina_actual + cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc); //TODO:deberia ser siempre la ultima, no?
// 					printf("- Memalloc->Pedir->Free: debo traer la pagina %i.\n", num_pag_a_traer);
// 					pagina = list_get(tabla_paginas->tabla_pag, num_pag_a_traer);
// 					puts("- Memalloc->Pedir->Free: traje la pagina");

// 					//TODO: la cargo en memoria

// 					memcpy(ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc), &new, SIZE_METADATA);
// 					printf("- Memalloc->Pedir->Free: cargue el nuevo segmento en la direccion logica %i, puntero %p.\n", ptr_potencial_segmento->nextAlloc, ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc));

// 					printf("- Memalloc->Pedir->Free: RETORNO DIR_LOG DE GUARDADO: %i. METADATA ARRANCA EN %i.\n", inicio_actual+SIZE_METADATA, inicio_actual);
// 					return inicio_actual + SIZE_METADATA;
// 				}
// 				else{
// 					puts("- Memalloc->Pedir->NoFree: el ultimo alloc esta ocupado. Genero nuevo metadata al principio de la pagina.");

// 					int num_primera_pagina_nueva = num_pagina_actual+1;
// 					pag_t* primera_pagina_nueva = list_get(tabla_paginas->tabla_pag, num_primera_pagina_nueva);
// 					printf("- Memalloc->Pedir->NoFree: agarre la pagina %i.\n", num_primera_pagina_nueva);

// 					int inicio_anterior = inicio_actual;
// 					inicio_actual = num_primera_pagina_nueva*configuracion.TAMANIO_PAGINAS;
// 					ptr_potencial_segmento->nextAlloc = inicio_actual;
// 					printf("- Memalloc->Pedir->NoFree: cambie el nextAlloc del segmento anterior a %i.\n", ptr_potencial_segmento->nextAlloc);

// 					HeapMetadata new;
// 					new.isFree = false;
// 					new.prevAlloc = inicio_anterior;

// 					if((tamanio + SIZE_METADATA) % configuracion.TAMANIO_PAGINAS == 0){//entra justo
// 						puts("- Memalloc->Pedir->NoFree: el nuevo metadata entra justo.");

// 						new.nextAlloc = LAST_METADATA;
// 						printf("- Memalloc->Pedir->NoFree: cree el nuevo metadata, con isFree = %s; prev = %i, next = %i.\n", (new.isFree?"true":"false"), new.prevAlloc, new.nextAlloc);

// 						memcpy(ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0), &new, SIZE_METADATA); //cargo la nueva metadata
// 						printf("- Memalloc->Pedir->NoFree: cargue el nuevo metadata en la direccion %i, puntero %p.\n", ptr_potencial_segmento->nextAlloc, ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0));
// 					}
// 					else{
// 						puts("- Memalloc->Pedir->NoFree: tengo que dividir en 2.");

// 						new.nextAlloc = inicio_actual + SIZE_METADATA + tamanio;
// 						printf("- Memalloc->Pedir->NoFree: cree el nuevo metadata, con isFree = %s; prev = %i, next = %i.\n", (new.isFree?"true":"false"), new.prevAlloc, new.nextAlloc);

// 						memcpy(ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0), &new, SIZE_METADATA);
// 						printf("- Memalloc->Pedir->NoFree: cargue el nuevo metadata en la direccion %i, puntero %p.\n", ptr_potencial_segmento->nextAlloc, ubicacion_nuevo_segmento(primera_pagina_nueva->marco, 0));

// 						int num_pagina_next = list_size(tabla_paginas->tabla_pag)-1;
// 						printf("- Memalloc->Pedir->NoFree: el metadata next esta en la pagina %i.\n", num_pagina_next);
// 						pag_t* pagina_next;
// 						if(num_primera_pagina_nueva != num_pagina_next){ //si el next queda en otra pagina
// 							pagina_next = list_get(tabla_paginas->tabla_pag, num_pagina_next);
// 							puts("- Memalloc->Pedir->NoFree: agarro la pagina");
// 							//TODO: la traigo a memoria
// 						} else pagina_next = primera_pagina_nueva;

// 						HeapMetadata next;
// 						next.isFree = true;
// 						next.prevAlloc = inicio_actual;
// 						next.nextAlloc = LAST_METADATA;
// 						printf("- Memalloc->Pedir->NoFree: cree el next metadata, con isFree = %s; prev = %i, next = %i.\n", (next.isFree?"true":"false"), next.prevAlloc, next.nextAlloc);

// 						memcpy(ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc), &next, SIZE_METADATA);
// 						printf("- Memalloc->Pedir->NoFree: cargue el next metadata en la direccion %i, puntero %p.\n", new.nextAlloc, ubicacion_nuevo_segmento(pagina_next->marco, new.nextAlloc));
// 					}
// 					printf("- Memalloc->Pedir->NoFree: RETORNO DIRECCION LOGICA %i.\n", ptr_potencial_segmento->nextAlloc+SIZE_METADATA);
// 					return ptr_potencial_segmento->nextAlloc+SIZE_METADATA; //retorno la direccion logica
// 				}
// 			}
// 		}

// // int desginar_PID(int cliente){
// //     int pid = recibir_PID(cliente);
// //     if(pid == PID_EMPTY) return crearID(&ids_memoria);
// //     return pid;
// // }

// void ejecutar_malloc(t_list* tabla, int cliente){

//     int size = recibir_tamanio_malloc(cliente);

////      if(list_size(tabla) == 0) crear_pagina(tabla); 
//     alloc_memory(tabla, size);
// }

// void memfree();


// 		puts("- Memalloc->While: este segmento no es valido para alocar. Pasando al proximo segmento.");
// 		//Si no entre a los if anteriores, significa que no encontre un hueco todavia. Tengo que pasar al siguiente alloc.
// 		if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
// 			//El siguiente metadata esta en otra pagina
// 			puts("- Memalloc->While: el siguiente metadata esta en otra pagina.");
// 			//Obtengo dir fisica de inicio de pagina nueva, luego el metadata esta en [direccion.segmento + configuracion.TAMANIO_PAGINA - inicio_actual]
// 			num_pagina_actual += cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc);
// 			printf("- Memalloc->While: tengo que buscar la pagina %i.\n", num_pagina_actual);
// // 			pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
// // 			//TODO: la cargo en memoria
// 		}

// 		inicio_actual = ptr_potencial_segmento->nextAlloc;
// 		printf("- Memalloc->While: el nuevo inicio sera %i.\n", inicio_actual);
// 		ptr_potencial_segmento = ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc); //TODO: REVISAR
// 		printf("- Memalloc->While: el nuevo puntero a revisar es el %p.\n", ptr_potencial_segmento);
// 	}

// 	return 0;
// }

// // void* ubicacion_nuevo_segmento(int num_marco, int dir_log){
// // 	return (ram.memoria + num_marco * configuracion.TAMANIO_PAGINAS + dir_log % configuracion.TAMANIO_PAGINAS);
// 	     // memoria              inicio                                      desplazamiento
// }


// int num_pagina_a_traer(uint32_t inicio){ //inicio del nuevo metadata
// 	int contador = 0;
// 	while (inicio / configuracion.TAMANIO_PAGINAS > 1){
// 		inicio -= configuracion.TAMANIO_PAGINAS;
// 		contador++;
// 	}
// 	return contador;
// }

// // HeapMetadata* primer_segmento(tab_pags* tabla){
// // 	dir_t direccion_logica;
// 	direccion_logica.segmento=0;
// 	direccion_logica.offset=0;

// 	dir_t direccion_fisica = convertir_a_df(tabla->tabla_pag, direccion_logica);
// 	int offset = offset_memoria(direccion_fisica);

// 	HeapMetadata* seg = ram.memoria + offset;
// 	return seg;
// }

// bool hay_cambio_de_pagina(int direc_actual, uint32_t next_alloc){
// 	for (int i=direc_actual+9; i <= next_alloc; i++) {
// 		if (i % configuracion.TAMANIO_PAGINAS == 0) {
// 			return true;
// 		}
// 	}
// 	return false;
// }

// int cant_cambios_de_pagina(int direc_actual, uint32_t next_alloc){
// 	int cant = 0;
// 	for (int i=direc_actual+9; i <= next_alloc; i++) {
// 		if (i % configuracion.TAMANIO_PAGINAS == 0) {
// 			cant++;
// 		}
// 	}
// 	return cant;
// }

// int memfree(int dir_log, int pid){
// 	printf("- Memfree: quiero liberar la direccion logica %i.\n", dir_log);
// 	puts("Memfree: busco la page table.");
// 	tab_pags* tabla_paginas = buscar_page_table(pid);
// 	int num_pag;
// // 	HeapMetadata* ptr_segmento = hallar_metadata(dir_log, tabla_paginas, &num_pag);
// // 	if(ptr_segmento == NULL){
// 		puts("- Memfree->ERROR: direccion logica invalida");
// 		return -1; //
// 	}
// 	printf("- Memfree: la pagina del segmento es la %i.\n", num_pag);

// // 	if(ptr_segmento->isFree){
// // 		puts("- Memfree: el segmento ya esta libre.");
// 		return -2;
// 	}

// 	pag_t* pagina_actual = list_get(tabla_paginas->tabla_pag, num_pag);
// 	ptr_segmento->isFree = true;
// 	puts("- Memfree: cambio el isFree a true.");

// 	int inicio_actual = dir_log;
// 	HeapMetadata *ptr_derecha, *ptr_izquierda;

// 	puts("- Memfree->Derecha: reviso a derecha.");
// 	if(ptr_segmento->nextAlloc != LAST_METADATA){
// 		if(hay_cambio_de_pagina(inicio_actual, ptr_segmento->nextAlloc)){
// 			puts("- Memfree->Derecha: el derecho esta en otra pagina.");
// pag_t* pagina_derecha = list_get(tabla_paginas->tabla_pag, num_pag + cant_cambios_de_pagina(inicio_actual, ptr_segmento->nextAlloc));
// 			ptr_derecha = ubicacion_nuevo_segmento(pagina_derecha->marco, ptr_segmento->nextAlloc);
// 		}else{
// 			ptr_derecha = ubicacion_nuevo_segmento(pagina_actual->marco, ptr_segmento->nextAlloc); //TODO: REVISAR
// 		}
// // 		printf("- Memfree->Derecha: el puntero a derecha esta en el puntero %p.\n", ptr_derecha);
// // 		if(ptr_derecha->isFree){
// 			puts("- Memfree->Derecha: el puntero a derecha esta libre. Procedo a liberarlo.");
// 			ptr_segmento->nextAlloc = ptr_derecha->nextAlloc;
// 		} else puts("- Memfree->Derecha: el puntero a derecha esta ocupado.");
// 	} else puts("- Memfree->Derecha: no existe puntero a derecha.");


// 	//reviso a izquierda TODO:checkear
// 	puts("- Memfree->Izquierda: reviso a izquierda.");
// 	if(ptr_segmento->prevAlloc != FIRST_METADATA){
// 		if(hay_cambio_de_pagina(ptr_segmento->prevAlloc, inicio_actual)){
// 			puts("- Memfree->Izquierda: el izquierdo esta en otra pagina.");
// 			pag_t* pagina_izquierda = list_get(tabla_paginas->tabla_pag, num_pag - cant_cambios_de_pagina(ptr_segmento->prevAlloc, inicio_actual));
// 			ptr_izquierda = ubicacion_nuevo_segmento(pagina_izquierda->marco, ptr_segmento->prevAlloc);
// 		}else{
// 			ptr_izquierda = ubicacion_nuevo_segmento(pagina_actual->marco, ptr_segmento->prevAlloc); //TODO: REVISAR
// 		}
// 		printf("- Memfree->Izquierda: el puntero a izquierda esta en el puntero %p.\n", ptr_izquierda);
// 		if(ptr_izquierda->isFree){
// 			puts("- Memfree->Izquierda: el puntero a izquierda esta libre. Procedo a eliminarlo.");
// 			ptr_izquierda->nextAlloc = ptr_segmento->nextAlloc;
// 		} else puts("- Memfree->Izquierda: el puntero a izquierda esta ocupado.");
// 	} else puts("- Memfree->Izquierda: no existe puntero a izquierda.");

// 	return 1;
// 	//Checkear por paginas desocupadas y liberarlas
// }

// bool pedir_memoria_a_swap(int tamanio){
// 	return true;
// }

// HeapMetadata* hallar_metadata(uint32_t dir_log, tab_pags* tabla, int* num_pag){
// 	HeapMetadata* potencial_segmento = primer_segmento(tabla);
// 	uint32_t inicio_actual = 0;
// 	int num_pagina_actual = 0;
// 	pag_t* pagina = list_get(tabla->tabla_pag, num_pagina_actual);
// 	while(inicio_actual != dir_log){
// 		printf("Memfree->Hallar Metadata: el inicio actual %i es distinto al que busco %i.\n", inicio_actual, dir_log);
// 		if(potencial_segmento->nextAlloc == LAST_METADATA) return NULL;

// 		if(hay_cambio_de_pagina(inicio_actual, potencial_segmento->nextAlloc)){
// 			num_pagina_actual += cant_cambios_de_pagina(inicio_actual, potencial_segmento->nextAlloc);
// 			printf("Memfree->Hallar Metadata: voy a buscar al proximo en la pagina %i.\n", num_pagina_actual);
// 			pagina = list_get(tabla->tabla_pag, num_pagina_actual);
// 			//TODO: la cargo en memoria
// 		}
// 		inicio_actual = potencial_segmento->nextAlloc;
// 		potencial_segmento = ubicacion_nuevo_segmento(pagina->marco, potencial_segmento->nextAlloc);
// 	}
// 	printf("Memfree->Hallar Metadata: encontre la metadata en la pagina %i.\n", num_pagina_actual);
// 	*num_pag = num_pagina_actual;
// 	return potencial_segmento;
// }

// void TEST_report_metadatas(int pid){
// 	puts("--COMIENZO REPORTE METADATA--");
// 	tab_pags* tabla_paginas = buscar_page_table(pid);
// 	if(tabla_paginas == NULL){
// 		puts("ERROR CARPINCHO INVALIDO");
// 		return;
// 	}
// 	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
// 	int inicio_actual = 0, num_pagina_actual=0;
// 	pag_t* pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
// 	int numero_magico, espacio_en_alloc;
// 	int num_meta = 0;
// 	puts("\tPAGINA 0:");
// 	while(1){

// 		numero_magico = (ptr_potencial_segmento->nextAlloc == LAST_METADATA ? (list_size(tabla_paginas->tabla_pag)*configuracion.TAMANIO_PAGINAS -1) : ptr_potencial_segmento->nextAlloc);

// 		espacio_en_alloc = numero_magico - SIZE_METADATA - inicio_actual +1;
// 		printf("\tMetadata %i:\tinicio\t\tprev\t\tnext\t\ttamanio\t\tisFree\n", num_meta++);
// 		printf("\t\t\t%i\t\t%i\t\t%i\t\t%i\t\t", inicio_actual, ptr_potencial_segmento->prevAlloc, ptr_potencial_segmento->nextAlloc, espacio_en_alloc);
// 		printf((ptr_potencial_segmento->isFree) ? "true\n" : "false\n");

// 		if(ptr_potencial_segmento->nextAlloc == LAST_METADATA){
// 			puts("--FIN REPORTE METADATA--\n\n");
// 			return;
// 		}

// 		if(ptr_potencial_segmento->nextAlloc==inicio_actual){
// 			puts("--ERROR DE LOOP, ABORTANDO!!--");
// 			return;
// 		}

// 		if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
// 			num_pagina_actual += cant_cambios_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc);
// 			pagina = list_get(tabla_paginas->tabla_pag, num_pagina_actual);
// 			printf("\tPAGINA %i:\n", num_pagina_actual);

// 		}
// 		inicio_actual = ptr_potencial_segmento->nextAlloc;
// 		ptr_potencial_segmento = ubicacion_nuevo_segmento(pagina->marco, ptr_potencial_segmento->nextAlloc);
// 	}
// }

void* memread(tab_pags* tabla, int dir_log, int tamanio){

    dir_t dl = traducir_dir_log(dir_log);
    return memoria_leer(tabla, dl, tamanio);
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
        page_use(dl.PAGINA, tabla);

        page_remaining_space = configuracion.TAMANIO_PAGINAS;
        bytes_remaining -= leer;
        leido += leer;
        dl.offset = 0;
        dl.PAGINA++;
    }

    return buffer;
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

void memwrite();

