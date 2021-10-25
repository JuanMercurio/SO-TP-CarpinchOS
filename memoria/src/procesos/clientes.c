#include "clientes.h"
#include "../esquema/paginacion.h"

#include <matelib/matelib.h>
#include <conexiones/conexiones.h>
#include <utils/utils.h>
#include <sys/socket.h>
#include <pthread.h>

void atender_proceso(void* arg){
    int cliente = *(int*)arg;
    free(arg);

    handshake(cliente, "memoria");
    ejecutar_proceso(cliente);
}

void ejecutar_proceso(int cliente) {

    while(1) //TODO: feo while
    {
        int operacion = recibir_operacion(cliente);
        switch (operacion)
        {
        case NEW_INSTANCE:
            iniciar_proceso(cliente);
            break;
        
        case MEMALLOC:
            break;

        case MEMFREE:
            break;

        case MEMREAD:
            break;

        case MEMWRITE:
            break;

        default:
            break;
        }
    }
}

void iniciar_proceso(int cliente){
    // checkear si puede iniciar
    // si no puede ser iniciado: pthread_exit(0)
    int pid = crearID(&ids_memoria);
    //recibir_instancia();
    tab_pags* tabla = malloc(sizeof(tab_pags));
    tabla->pid = pid;
    tabla->tabla_pag = list_create();

    add_new_page_table(tabla);
    // enviar_instancia();
 }
