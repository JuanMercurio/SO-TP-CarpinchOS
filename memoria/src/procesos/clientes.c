#include "clientes.h"
#include "operaciones.h"
#include "../esquema/paginacion.h"
#include <matelib/matelib.h>
#include <mensajes/mensajes.h>
#include <utils/utils.h>
#include <sys/socket.h>
#include <pthread.h>

void atender_proceso(void* arg){
    int cliente = *(int*)arg;
    free(arg);

    handshake(cliente, "memoria");
    ejecutar_proceso(cliente);
}

void ejecutar_proceso(int cliente)
{   
    char* recibido;
    bool conectado = true;
    int pid = PID_EMPTY; //con esta variable no es necesario enviar en todos los mensaje el pid
                            /*
                            este hilo que ejecuta al proceso vive con la instancia y recibira de la conexion cliente siempre los mensajes de un carpincho
                            por eso en las acciones de mem se envia solo el int que corresponde alcampo informacion de tal accioin y no el pid
                            nota: los int tiene un rango de desde -2.147.483.648 hasta 2.147.483.647 no veo la necesidad de enviar el tamaño de un int ya que
                            queda implicito en eltipo de dato. */
    t_list *tabla;
    while (conectado) //TODO: feo while
    {
        int operacion = recibir_operacion(cliente);
        switch (operacion)
        {
        case NEW_INSTANCE:
            pid = desginar_PID(cliente);
            tabla = iniciar_paginas(cliente, pid);
            // enviar_PID(cliente);
            //enviar info a carpincho
            break;

        case MEMALLOC:
            ejecutar_malloc(tabla, cliente);
            /* RECIBE EDE KERNEL EL TAMAÑO A ALLOCAR
                DEVUELVE UN INT CON LA DL O UN -1 PARA DENEGAR ASIGNACION
             */
            break;

        case MEMFREE:
            /* RECIBE EDE KERNEL EL TAMAÑO A LIBERAR
                DEVUELVE UN INT CON LA DL O UN -5 PARA DENEGAR 
             */

            break;

        case MEMREAD:
            /* recibe una estructura con el int del ptr origen un void* que es un char* con el nombre de una io (segun ejemplo)
         Se usa para obtener información de la memoria. Con la dirección lógica que nos proveen y  la tabla de páginas hacemos la traducción a direcciones
          físicas y retornamos el contenido. 
        En caso de que se quiera leer una posición de memoria errónea, se deberá devolver el error MATE_READ_FAULT definido en el
         enum mate_errors de la matelib correspondiente con el valor -6.
*/
            break;

        case MEMWRITE:
            break;

        case NEW_INSTANCE_KERNEL:
            pid = recibir_operacion(cliente); // del otro lado se envia solamente el cod op y el pid
            tabla = iniciar_paginas(cliente, pid);
            break;

        case MATE_CLOSE: recibir_mensaje(cliente); /* DEBERA ELIMINAR LAS TABLAS DEL PROCESO CERRADO?? */
         close(cliente);

        default:
            break;
        }
    }
}

t_list* iniciar_paginas(int cliente, int pid){
    // checkear si puede iniciar
    // si no puede ser iniciado: pthread_exit(0)
    tab_pags* tabla = malloc(sizeof(tab_pags));
    tabla->pid = pid;
    tabla->tabla_pag = list_create();

    add_new_page_table(tabla);
    // enviar_instancia();  ?

    return tabla->tabla_pag;
}