#include "main.h"
#include "signals/signal.h"
#include "configuracion/config.h"
#include "esquema/tlb.h"
#include "esquema/paginacion.h"
#include "procesos/clientes.h"

#include <stdint.h>
#include <utils/utils.h> 
#include <conexiones/conexiones.h>

int main(int argc, char* argv[]) {

   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    
   
   obtener_config();
   iniciar_signals();
   iniciar_paginacion();
   iniciar_tlb();

   administrar_clientes(configuracion.IP, configuracion.PUERTO, (void*)&atender_proceso);

   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}

void atender_cliente(void* arg){
   int cliente = *(int*)arg;
   free(arg);

   printf("Se conecto usando el socket %d\n", cliente);
}


alocar_memoria(p, size){ //quizas al igual que antes, el carpincho se guarda local en el hilo? entonces no habría que pasarlo como parametro

	tabla_paginas = obtener_tabla_paginas(p);
	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
	int inicio_actual = 0;

	do{

		if(ptr_potencial_segmento->isFree && ptr_potencial_segmento->nextAlloc - SIZE_METADATA - inicio_actual >= size) {
			//encontre el segmento donde entra
			//ahora debo evaluar si entra justo o si tengo que dividirlo en 2 partes

			if(ptr_potencial_segmento->nextAlloc - SIZE_METADATA - inicio_actual == size){
				//entra justo
				ptr_potencial_segmento->isFree = false;
				return inicio_actual; //retorno la direccion logica
			}

			//tengo que dividir en 2                   			DUDA: que pasa si el lugar que queda es menor al tamaño de un metadata?
			ptr_potencial_segmento->isFree = false;
			int aux_next_alloc = ptr_potencial_segmento->nextAlloc;
			ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + size;

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

