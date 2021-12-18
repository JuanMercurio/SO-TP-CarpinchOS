#include "operaciones.h"

#include "../configuracion/config.h"
#include "../esquema/paginacion.h"
#include "../esquema/paginacion.h"

#include <mensajes/mensajes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/utils.h>
#include "../signals/signal.h"

#define SIZE_METADATA sizeof(HeapMetadata)

int memalloc(tab_pags* tabla, int tamanio){

    int pid = tabla->pid;
    tab_pags* tabla_paginas = tabla;

	printf("- Memalloc: quiero reservar %i para el carpincho %i.\n", tamanio, pid);
	log_info(logger_memoria, "-- MEMALLOC -- PID %d | TAM %d", pid, tamanio);

	if(tamanio==0){
		puts("- Memalloc: no se puede reservar espacio 0. Abortando.");
		log_info(logger_memoria, "-- FIN MEMALLOC --\n");
		return -2;
	}

    if (tabla_paginas->tabla_pag->head == NULL) {
		printf("Tengo que iniciar las paginas\n"); 
		log_info(logger_memoria, "Inicio las paginas.");
		if (heap_init(tabla) == -1) return -1; 
	}

	printf("- Memalloc: encontre la pagina del carpincho %i.\n", tabla_paginas->pid);
	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
	printf("- Memalloc: encontre el primer segmento, que tiene como nextAlloc al %i, y que su valor de isFree es %i, es decir ", ptr_potencial_segmento->nextAlloc, ptr_potencial_segmento->isFree);
	printf((ptr_potencial_segmento->isFree) ? "true.\n" : "false.\n");
	int inicio_actual = 0, num_pagina_actual=0, inicio_previo = FIRST_METADATA;
	puts("- Memalloc: entro al while.");
	int numero_magico; //si nextAlloc es 0, entonces esta variable vale la ultima direccion de memoria+1 (por ejemplo 128), sino vale nextAlloc
	int espacio_en_alloc;
	while(1){
		int prev = ptr_potencial_segmento->prevAlloc;
		int next = ptr_potencial_segmento->nextAlloc;
		if(((prev > next) && (next)!=LAST_METADATA && prev >= 0) || (next <= inicio_actual && next != LAST_METADATA)) {
			printf("ERROR.\n");
			printf("ptr_protencial_segmento: \tprev %d   |   next %d\n", prev, ptr_potencial_segmento->nextAlloc);
			printf("inicio_actual: %d\n", inicio_actual);
			generar_dump();
			abort();
		}

		//  ****BUSCO SI ENTRA****
		printf("- Memalloc->While->CUENTA:\n");
		numero_magico = (ptr_potencial_segmento->nextAlloc == LAST_METADATA ? (list_size(tabla_paginas->tabla_pag)*configuracion.TAMANIO_PAGINAS) : ptr_potencial_segmento->nextAlloc);
		espacio_en_alloc = numero_magico - SIZE_METADATA - inicio_actual;
		printf("\t\tsegmento->prevAlloc: %i.\n", ptr_potencial_segmento->prevAlloc);
		printf("\t\tsegmento->nextAlloc: %i.\n", numero_magico);
		printf("\t\tsize_metadata: %i.\n", SIZE_METADATA);
		printf("\t\tinicio_actual %i.\n", inicio_actual);
		printf("\t\tESPACIO EN ALLOC: %i.\n", espacio_en_alloc);
		printf("\t\tTAMANIO: %i.\n", tamanio);

		if(ptr_potencial_segmento->isFree){
			if(espacio_en_alloc == tamanio){
				puts("-Memalloc->While: entra justo.");
				log_info(logger_memoria, "Encontre un alloc que entra justo, direccion %d. Procedo a sobreescribirlo.", inicio_actual);
				ptr_potencial_segmento->isFree = false;
				memoria_escribir_por_dirlog(tabla, inicio_actual, ptr_potencial_segmento, SIZE_METADATA);
				printf("-Memalloc->While: cambio isFree a false y devuelvo direccion de inicio %i.\n", inicio_actual+SIZE_METADATA);

				if (ptr_potencial_segmento->nextAlloc == LAST_METADATA) { 
					log_info(logger_memoria, "Pido Paginas a Swap.");
					if(pedir_paginas_a_swap(tabla, 1) == -1) {
						puts("Swap me denego la pagina");
						log_info(logger_memoria, "Swap me denego la pagina.");
						log_info(logger_memoria, "-- FIN MEMALLOC --\n");
						free(ptr_potencial_segmento);
						return inicio_actual + SIZE_METADATA;
					}

					HeapMetadata new  = {0};
					new.nextAlloc = LAST_METADATA;
					new.prevAlloc = inicio_actual;
					new.isFree = 1;

					printf("new: \tprev %d   |   next %d\n", new.prevAlloc, new.nextAlloc);

					ptr_potencial_segmento->nextAlloc = numero_magico;
					log_info(logger_memoria, "Actualizo el nextAlloc del segmento alocado.");
					memoria_escribir_por_dirlog(tabla, inicio_actual, ptr_potencial_segmento, SIZE_METADATA);
					paginas_agregar(1, tabla);
					log_info(logger_memoria, "Cargo el nuevo metadata.");
					memoria_escribir_por_dirlog(tabla, ptr_potencial_segmento->nextAlloc, &new, SIZE_METADATA);
				}
				log_info(logger_memoria, "-- FIN MEMALLOC --\n");
				free(ptr_potencial_segmento);
				return inicio_actual + SIZE_METADATA;
			}

			if(espacio_en_alloc >= tamanio+SIZE_METADATA) { 
				printf("\t\tComo isFree es %i y %i >= %i (%i + %i), entro en el if.\n", ptr_potencial_segmento->isFree, espacio_en_alloc, tamanio+SIZE_METADATA, tamanio, SIZE_METADATA);
				printf("- Memalloc->While: el segmento con inicio en %i es valido para alocar. Esta en la pagina %i.\n", inicio_actual, num_pagina_actual);
				printf("- Memalloc->While: SU PUNTERO ES %p.\n", ptr_potencial_segmento);

				puts("- Memalloc->While: tengo que dividir en 2.");


				puts("- Memalloc->While: reviso que el otro metadata no tenga que ir en una nueva pagina.");
				int dir_ultimo_byte_alloc_actual = inicio_actual + SIZE_METADATA + tamanio - 1;
				int dir_ultimo_byte_nuevo_meta = inicio_actual + tamanio + SIZE_METADATA*2 - 1;
                if (ptr_potencial_segmento->nextAlloc == LAST_METADATA && hay_cambio_de_pagina(dir_ultimo_byte_alloc_actual, dir_ultimo_byte_nuevo_meta)){
                    puts("- Memalloc->While: el nuevo metadata no entra en la misma pagina.");

					int bytes_faltantes = SIZE_METADATA - (espacio_en_alloc-tamanio);
                    int paginas_a_agregar = bytes_faltantes / configuracion.TAMANIO_PAGINAS;
					if (bytes_faltantes % paginas_a_agregar != 0) paginas_a_agregar++;
                    printf("- Memalloc->While: tengo que agregar %d mas.", paginas_a_agregar);

                    if(pedir_paginas_a_swap(tabla_paginas, paginas_a_agregar) == -1){
                        printf("- Memalloc->While: no recibi las paginas. retorno -1.");
						log_info(logger_memoria, "-- FIN MEMALLOC --\n");
						free(ptr_potencial_segmento);
                        return -1;
                    }

                    puts("- Memalloc->While: recibi las paginas.");
                    paginas_agregar(paginas_a_agregar, tabla);
                }


				printf("- Memalloc->While: ptr_potencial_segmento->isFree es %d\n", ptr_potencial_segmento->isFree);
				ptr_potencial_segmento->isFree = false;
				printf("- Memalloc->While: ptr_potencial_segmento->isFree es %d\n", ptr_potencial_segmento->isFree);
				ptr_potencial_segmento->isFree = false;
				int aux_next_alloc = ptr_potencial_segmento->nextAlloc;
				ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;

				memoria_escribir_por_dirlog(tabla, inicio_actual, ptr_potencial_segmento, SIZE_METADATA);

				HeapMetadata new;
				new.prevAlloc = inicio_actual;
				new.nextAlloc = aux_next_alloc;
				new.isFree = true;

				HeapMetadata* next;

				if(new.nextAlloc!=LAST_METADATA) {
					puts("- Memalloc->While: existe metadata next");

					next = memoria_leer_por_dirlog(tabla_paginas, new.nextAlloc, SIZE_METADATA);
					next->prevAlloc = ptr_potencial_segmento->nextAlloc;
					memoria_escribir_por_dirlog(tabla_paginas, new.nextAlloc, next, SIZE_METADATA);

					free(next);

				} else puts("- Memalloc->While: no existe metadata next");

				memoria_escribir_por_dirlog(tabla_paginas, ptr_potencial_segmento->nextAlloc, &new, SIZE_METADATA);

				log_info(logger_memoria, "-- FIN MEMALLOC --\n");
				printf("- Memalloc->While: RETORNO DIR_LOG DE GUARDADO: %i. METADATA ARRANCA EN %i.\n", inicio_actual+SIZE_METADATA, inicio_actual);
				free(ptr_potencial_segmento);
				return inicio_actual + SIZE_METADATA;
			}
		} else puts("Segmento no esta libre.");



		//  ****BUSCO SI ES EL ULTIMO METADATA****

		if(ptr_potencial_segmento->nextAlloc == LAST_METADATA){
			puts("- Memalloc->While: llegue al final y no encontre nada");

			printf("- Memalloc->Pedir: estoy en la pagina numero %i.\n", num_pagina_actual);
			printf("- Memalloc->Pedir: mi inicio actual es %i.\n", inicio_actual);

			puts("- Memalloc->Pedir: AGREGO PAG NUEVA.");

			int bytes_faltantes;
			if(ptr_potencial_segmento->isFree == true){
				bytes_faltantes = tamanio - espacio_en_alloc;
				if(bytes_faltantes % configuracion.TAMANIO_PAGINAS != 0)	bytes_faltantes += SIZE_METADATA;
			} else {
				bytes_faltantes = tamanio + SIZE_METADATA;
				if(bytes_faltantes % configuracion.TAMANIO_PAGINAS != 0)	bytes_faltantes += SIZE_METADATA;
			}

			int paginas_faltantes = bytes_faltantes / configuracion.TAMANIO_PAGINAS;
			if (bytes_faltantes % configuracion.TAMANIO_PAGINAS != 0) paginas_faltantes++;

			printf("NECESITO %d BYTES, QUE SE TRADUCEN A %d PAGINAS\n", bytes_faltantes, paginas_faltantes);

			if(pedir_paginas_a_swap(tabla_paginas, paginas_faltantes) == -1){
				log_info(logger_memoria, "-- FIN MEMALLOC --\n");
				return -1;
			}
			else{
				printf("-  Memalloc->Pedir->Free: RECIBI %d PAGINAS DE SWAP.\n", paginas_faltantes);
				if(ptr_potencial_segmento->isFree) {
					puts("- Memalloc->Pedir->Free: el ultimo alloc esta libre. Lo expando.");
					numero_magico = (ptr_potencial_segmento->nextAlloc == LAST_METADATA ? (list_size(tabla_paginas->tabla_pag)*configuracion.TAMANIO_PAGINAS) : ptr_potencial_segmento->nextAlloc);
					espacio_en_alloc = numero_magico - SIZE_METADATA - inicio_actual;
					if(tamanio-espacio_en_alloc % configuracion.TAMANIO_PAGINAS == 0){ 
						ptr_potencial_segmento->isFree=false;
						memoria_escribir_por_dirlog(tabla_paginas, inicio_actual, ptr_potencial_segmento, SIZE_METADATA);
						free(ptr_potencial_segmento);
						log_info(logger_memoria, "-- FIN MEMALLOC --\n");
						return inicio_actual+SIZE_METADATA;
					}
					puts("- Memalloc->Pedir->Free: tengo que dividir en 2.");

					printf("ESPACIO_EN_ALLOC: %i", espacio_en_alloc);
					ptr_potencial_segmento->isFree = false;
					ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;

					memoria_escribir_por_dirlog(tabla, inicio_actual, ptr_potencial_segmento, SIZE_METADATA);

					HeapMetadata new;
					new.prevAlloc = inicio_actual;
					new.nextAlloc = LAST_METADATA;
					new.isFree = true;

					printf("Esto deberia ser 18: %d\n", ptr_potencial_segmento->nextAlloc);
					puts("- Memalloc->Pedir->Free: el metadata esta en otra pagina, obviamente.");

					paginas_agregar(paginas_faltantes, tabla);
					memoria_escribir_por_dirlog(tabla, ptr_potencial_segmento->nextAlloc, &new, SIZE_METADATA);

					printf("- Memalloc->Pedir->Free: RETORNO DIR_LOG DE GUARDADO: %i. METADATA ARRANCA EN %i.\n", inicio_actual+SIZE_METADATA, inicio_actual);
					free(ptr_potencial_segmento);
					log_info(logger_memoria, "-- FIN MEMALLOC --\n");
					return inicio_actual + SIZE_METADATA;
				}
				else{
					puts("- Memalloc->Pedir->NoFree: el ultimo alloc esta ocupado. Genero nuevo metadata al principio de la pagina.");

					int num_primera_pagina_nueva = num_pagina_actual+1; 

					
					inicio_actual = num_primera_pagina_nueva*configuracion.TAMANIO_PAGINAS;
					ptr_potencial_segmento->nextAlloc = inicio_actual;
					memoria_escribir_por_dirlog(tabla, inicio_previo, ptr_potencial_segmento, SIZE_METADATA);


					printf("- Memalloc->Pedir->NoFree: cambie el nextAlloc del segmento anterior a %i.\n", ptr_potencial_segmento->nextAlloc);

					HeapMetadata new;
					new.isFree = false;
					new.prevAlloc = inicio_previo;
					dir_t dir_new;
					dir_new.offset=0;
					dir_new.PAGINA=num_primera_pagina_nueva;

					if((tamanio + SIZE_METADATA) % configuracion.TAMANIO_PAGINAS == 0){//entra justo
						puts("- Memalloc->Pedir->NoFree: el nuevo metadata entra justo.");

						new.nextAlloc = LAST_METADATA;
						printf("- Memalloc->Pedir->NoFree: cree el nuevo metadata, con isFree = %s; prev = %i, next = %i.\n", (new.isFree?"true":"false"), new.prevAlloc, new.nextAlloc);
						paginas_agregar(paginas_faltantes, tabla);
					}
					else{
						puts("- Memalloc->Pedir->NoFree: tengo que dividir en 2.");

						new.nextAlloc = inicio_actual + SIZE_METADATA + tamanio;
						printf("- Memalloc->Pedir->NoFree: cree el nuevo metadata, con isFree = %s; prev = %i, next = %i.\n", (new.isFree?"true":"false"), new.prevAlloc, new.nextAlloc);

						HeapMetadata next;
						next.isFree = true;
						next.prevAlloc = inicio_actual;
						next.nextAlloc = LAST_METADATA;
						printf("- Memalloc->Pedir->NoFree: cree el next metadata, con isFree = %s; prev = %i, next = %i.\n", (next.isFree?"true":"false"), next.prevAlloc, next.nextAlloc);
						
						paginas_agregar(paginas_faltantes, tabla);
						memoria_escribir_por_dirlog(tabla_paginas, new.nextAlloc, &next, SIZE_METADATA);
					}

					
					memoria_escribir(tabla_paginas, dir_new, &new, SIZE_METADATA);
					printf("- Memalloc->Pedir->NoFree: RETORNO DIRECCION LOGICA %i.\n", ptr_potencial_segmento->nextAlloc+SIZE_METADATA);
					free(ptr_potencial_segmento);
					log_info(logger_memoria, "-- FIN MEMALLOC --\n");
					return ptr_potencial_segmento->nextAlloc+SIZE_METADATA; //retorno la direccion logica
				}
			}
		}




		puts("- Memalloc->While: este segmento no es valido para alocar. Pasando al proximo segmento.");

		inicio_previo = inicio_actual;
		inicio_actual = ptr_potencial_segmento->nextAlloc;
		printf("- Memalloc->While: el nuevo inicio sera %i.\n", inicio_actual);
		free(ptr_potencial_segmento);
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

	log_info(logger_memoria, "Agarro el primer puntero y empiezo a recorrer");
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
	
	log_info(logger_memoria, "-- MEMFREE -- PID %d | DIR %d");
    tab_pags* tabla_paginas = tabla;

	printf("- Memfree: quiero liberar la direccion logica %i | PID: %d.\n", dir_log, tabla->pid) ;

	if(dir_log >= configuracion.TAMANIO){
		puts("- Memfree->ERROR: direccion logica imposible.");
		log_info(logger_memoria, "ERROR direccion logica IMPOSIBLE.");
		log_info(logger_memoria, "-- FIN MEMFREE --");
		return -5;
	}
	puts("Memfree: busco la page table.");

	HeapMetadata* ptr_segmento = memoria_leer_por_dirlog(tabla, dir_log - sizeof(HeapMetadata), sizeof(HeapMetadata));

	if(ptr_segmento == NULL){
		puts("- Memfree->ERROR: direccion logica invalida");
		log_info(logger_memoria, "ERROR direccion logica INVALIDA.");
		log_info(logger_memoria, "-- FIN MEMFREE --");
		return -5; //
	}
	printf("- Memfree: isFree = %d\n", ptr_segmento->isFree);
	if(ptr_segmento->isFree == 1){
		puts("- Memfree: el segmento ya esta libre.");
		log_info(logger_memoria, "ERROR segmento YA ESTA LIBRE.");
		log_info(logger_memoria, "-- FIN MEMFREE --");
		return -5;
	}

	ptr_segmento->isFree = true;
	puts("- Memfree: cambio el isFree a true.");

	int inicio_actual_metadata = dir_log - SIZE_METADATA;
	HeapMetadata *ptr_derecha, *ptr_izquierda;

	puts("- Memfree->Derecha: reviso a derecha.");
	if(ptr_segmento->nextAlloc != LAST_METADATA){
		log_info(logger_memoria, "Reviso a derecha.");
		ptr_derecha = memoria_leer_por_dirlog(tabla_paginas, ptr_segmento->nextAlloc, SIZE_METADATA);
		if(ptr_derecha->isFree){
			puts("- Memfree->Derecha: el puntero a derecha esta libre. Procedo a liberarlo.");
			log_info(logger_memoria, "Libero puntero a derecha.");
			ptr_segmento->nextAlloc = ptr_derecha->nextAlloc;
			if (ptr_segmento->nextAlloc != LAST_METADATA) {
				HeapMetadata *ptr_next = memoria_leer_por_dirlog(tabla, ptr_segmento->nextAlloc, SIZE_METADATA);
				ptr_next->prevAlloc = inicio_actual_metadata;
				log_info(logger_memoria, "Actualizo puntero next.");
				memoria_escribir_por_dirlog(tabla, ptr_segmento->nextAlloc, ptr_next, SIZE_METADATA);
			}
		} else puts("- Memfree->Derecha: el puntero a derecha esta ocupado.");
		free(ptr_derecha);
	} else puts("- Memfree->Derecha: no existe puntero a derecha.");


	puts("- Memfree->Izquierda: reviso a izquierda.");
	if(ptr_segmento->prevAlloc != FIRST_METADATA){
		log_info(logger_memoria, "Reviso a izquierda.");
		ptr_izquierda = memoria_leer_por_dirlog(tabla_paginas, ptr_segmento->prevAlloc, SIZE_METADATA);
		if(ptr_izquierda->isFree){
			puts("- Memfree->Izquierda: el puntero a izquierda esta libre. Procedo a eliminarlo.");
			log_info(logger_memoria, "Libero puntero a izquierda.");
			// ptr_izquierda->nextAlloc = ptr_segmento->nextAlloc;
			inicio_actual_metadata = ptr_segmento->prevAlloc;
			ptr_segmento->prevAlloc = ptr_izquierda->prevAlloc;
			// ptr_segmento = ptr_izquierda;
			if(ptr_segmento->prevAlloc != FIRST_METADATA){
				HeapMetadata *ptr_prev = memoria_leer_por_dirlog(tabla, ptr_segmento->prevAlloc, SIZE_METADATA);
				ptr_prev->nextAlloc = inicio_actual_metadata;
				log_info(logger_memoria, "Actualizo puntero prev.");
				memoria_escribir_por_dirlog(tabla, ptr_segmento->prevAlloc, ptr_prev, SIZE_METADATA);
			}
		} else puts("- Memfree->Izquierda: el puntero a izquierda esta ocupado.");
		free(ptr_izquierda);
	} else puts("- Memfree->Izquierda: no existe puntero a izquierda.");

	log_info(logger_memoria, "Cargo el puntero.");
	memoria_escribir_por_dirlog(tabla, inicio_actual_metadata, ptr_segmento, SIZE_METADATA);


	free(ptr_segmento);
	puts("- Memfree->Liberar: reviso si hay que liberar paginas.");
	log_info(logger_memoria, "Reviso si hay que liberar paginas.");

	if(ptr_segmento->nextAlloc == LAST_METADATA){
		int espacio_en_alloc = list_size(tabla_paginas->tabla_pag)*configuracion.TAMANIO_PAGINAS - SIZE_METADATA - inicio_actual_metadata;
		if(espacio_en_alloc >= configuracion.TAMANIO_PAGINAS){
			//tengo que liberar al menos una pagina
			int cant_pags_a_liberar = espacio_en_alloc/configuracion.TAMANIO_PAGINAS;
			for(int i=0; i<cant_pags_a_liberar; i++){
				int ultima = list_size(tabla_paginas->tabla_pag)-1;
				pag_t *pagina = list_remove(tabla_paginas->tabla_pag, ultima);
				pagina_liberar(pagina, ultima, tabla->pid);
				printf("Elimine la pagina %d\n", ultima);
			}
		}else
		printf("NOhayque liberar paginas\n");
		tablas_imprimir_saturno();
		printf("MemFree FIN - El proceso tiene %d paginas\n", list_size(tabla->tabla_pag));
	}else{
		printf("NOhayque liberar paginas\n");
	}

	log_info(logger_memoria, "-- FIN MEMFREE --\n");


	return 1;
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

void* memread(tab_pags* tabla, int dir_log, int tamanio){
    // dir_t dl = traducir_dir_log(dir_log);
	log_info(logger_memoria, "-- MEMREAD -- PID %d, DIR %d | TAM %d", tabla->pid, dir_log, tamanio);
	dir_t dl = decimal_a_dl(dir_log);

    // if(!alloc_valido(dl, tabla, tamanio)) return NULL; 
	void* read = memoria_leer(tabla, dl, tamanio);
	log_info(logger_memoria, "-- FIN MEMREAD --\n");
    return read;
}

int memwrite(tab_pags* tabla, int dir_log, void* contenido, int tamanio){
	log_info(logger_memoria, "-- MEMWRITE -- PID %d | DIR %d | TAM %d", tabla->pid, dir_log, tamanio);
    // dir_t dl = traducir_dir_log(dir_log);
	dir_t dl = decimal_a_dl(dir_log);

    // if(!alloc_valido(dl, tabla, tamanio)) return MATE_WRITE_FAULT;

	int done = memoria_escribir(tabla, dl, contenido, tamanio);
	log_info(logger_memoria, "-- FIN MEMWRITE --\n");
    return done;
}

void* memoria_leer(tab_pags* tabla, dir_t dl, int tamanio){
	printf("\nLECTURA - PAG %d - OFFSET: %d\n", dl.PAGINA, dl.offset);
	log_info(logger_memoria, "LECTURA - PID %d | PAG %d | OFFSET %d", tabla->pid, dl.PAGINA, dl.offset);
    if(!read_verify_size(tabla, dl, tamanio)) return NULL; 

    int leido = 0;
    int bytes_remaining = tamanio;
    int page_remaining_space = configuracion.TAMANIO_PAGINAS - dl.offset;

    void* buffer = malloc(tamanio);
    while(bytes_remaining > 0)
    {
        int marco = nro_marco(dl.PAGINA, tabla, READ);
		log_info(logger_memoria, "ESTOY LEYENDO DESDE EL MARCO %d", marco);
		tablas_loggear_saturno();

		if (marco == -1) { 
			printf("No se encontro el marco\n");
			log_info(logger_memoria,"No se encontro el marco\n");
			abort();
		}

        dir_t df = { marco, dl.offset };
        int leer = min_get(bytes_remaining, page_remaining_space);

        memcpy(buffer + leido, ram.memoria + offset_memoria(df), leer);

		// pag_t* pagina = list_get(tabla->tabla_pag, dl.PAGINA);
        // page_use(tabla->pid, marco, pagina, dl.PAGINA, READ);

        page_remaining_space = configuracion.TAMANIO_PAGINAS;
        bytes_remaining -= leer;
        leido += leer;
        dl.offset = 0;
        dl.PAGINA++;
    }
	
	log_info(logger_memoria, "Fin lectura");
    return buffer;
}

int memoria_escribir(tab_pags* tabla, dir_t dl, void* contenido, int tamanio){
    if(!read_verify_size(tabla, dl, tamanio)) return MATE_WRITE_FAULT;
	printf("ESCRITURA - PID %d - PAG %d - OFFSET: %d", tabla->pid, dl.PAGINA, dl.offset);
	log_info(logger_memoria, "ESCRITURA - PID %d | PAG %d | OFFSET %d | TAM %d", tabla->pid, dl.PAGINA, dl.offset, tamanio);

    int written = 0;
    int bytes_remaining = tamanio;
    int bytes_remaining_space = configuracion.TAMANIO_PAGINAS - dl.offset;
	printf("-----------------------------bytes reamin %d\n",bytes_remaining);
	printf("-----------------------------bytes reamin space %d\n",bytes_remaining_space);
    while(bytes_remaining > 0)
    {
        int marco = nro_marco(dl.PAGINA, tabla, WRITE);
		if(marco == -1){ printf("No se encontro el marco\n");
			log_info(logger_memoria,"No se encontro el marco\n");
		abort();
		}
		
        dir_t df = { marco, dl.offset };
        int bytes_to_write = min_get(bytes_remaining, bytes_remaining_space);
		printf("Voy a escribir %d bytes\n", bytes_to_write);
		//log_info(logger_memoria,"Voy a escribir %d bytes en la pagina %d.", bytes_to_write, dl.PAGINA);
		printf("El offset es %d\n", offset_memoria(df));
		// log_info(logger_memoria,"El offset es %d", offset_memoria(df));


        memcpy(ram.memoria + offset_memoria(df), contenido + written, bytes_to_write);
		
		// pag_t* pagina = list_get(tabla->tabla_pag, dl.PAGINA);
        // page_use(tabla->pid, marco, pagina, dl.PAGINA, WRITE);

        bytes_remaining_space = configuracion.TAMANIO_PAGINAS;
        bytes_remaining -= bytes_to_write;
        written += bytes_to_write;
        dl.offset = 0;
        dl.PAGINA++;
    }
	log_info(logger_memoria, "Fin escritura.");
    return 0;
}

bool alloc_valido(dir_t dl, tab_pags* t, int tamanio){
    dir_t heap_location = heap_get_location(dl, t);
    HeapMetadata* data = (HeapMetadata*) memoria_leer(t, heap_location, sizeof(HeapMetadata));

	bool libre = data->isFree;
	bool lectura_valida = heap_lectura_valida(data, dl, tamanio);

	free(data);

    return libre && lectura_valida;
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
	 dir_t dir;
	 dir.PAGINA = dl/configuracion.TAMANIO_PAGINAS;
	 dir.offset = dl%configuracion.TAMANIO_PAGINAS;
	//int bin_dl = decimal_a_binario(dl);
	//dir_t dir = traducir_dir_log(bin_dl);
	return memoria_leer(tabla, dir, tamanio);
}

int memoria_escribir_por_dirlog(tab_pags* tabla, int dl, void* contenido, int tamanio){
	 dir_t dir;
	 dir.PAGINA = dl/configuracion.TAMANIO_PAGINAS;
	 dir.offset = dl%configuracion.TAMANIO_PAGINAS;
	printf("-----------------------------tamaño %d\n", tamanio);
	//int bin = decimal_a_binario(dl);
	//dir_t dir = traducir_dir_log(bin);
	printf("-----------------------------dirrrrr offset %d\n",dir.offset);
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

	log_info(logger_alg, "ALG - PID: %d - PAG: %d", tabla->pid, 0);
	int inicio = pagina_iniciar(tabla);
	if (inicio == -1) return -1;

    memoria_escribir(tabla, dl, data, SIZE_METADATA);

	free(data);
    
	return 0;
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

int pedir_paginas_a_swap(tab_pags* tabla, int c_paginas){
	int paginas_totales = list_size(tabla->tabla_pag);
	int pagina = paginas_totales;
	int estado;

	enviar_int(swap, SOLITIDUC_MUCHAS_PAGINAS);
	enviar_int(swap, tabla->pid);
	enviar_int(swap, c_paginas);
	for (int i=0; i < c_paginas; i++) {
		enviar_int(swap, i+pagina);
	}

	estado = recibir_int(swap);
	return estado;
}

int paginas_agregar(int new_pags, tab_pags* tabla)
{
	for (int i=0; i < new_pags; i++) {
		int inicio = pagina_iniciar(tabla);
		if (inicio == -1) return -1;
	}

	return 0;
}