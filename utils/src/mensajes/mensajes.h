#ifndef _MENSAJES_
#define _MENSAJES_

#include "../conexiones/conexiones.h"


typedef enum{
    KERNEL,
    MEMORIA,
} cod_server;


typedef enum{
    SOLICITUD_INICIO,
    INICIO,
    SOLICITUD_PAGINA,
    ESCRIBIR_PAGINA,
    INICIO_CONFIG, 
}cod_swamp;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	int codigo_operacion;
	t_buffer* buffer;
} t_paquete;

/* 
    @NAME:  handshake
    @DESC:  dado un cliente el server le manda el modulo que es 
 */
void handshake(int cliente, cod_server modulo);

/* 
    @NAME:  enviar_mensaje
    @DESC:  enviar a un cliente un mensaje
 */
void enviar_mensaje(int cliente, char* mensaje);

/* 
    @NAME:  enviar_int
    @DESC:  envia un int al socket
 */
void enviar_int(int socket, int codigo);

/* 
    @NAME:  recibir_operacion
    @DESC:  recibe la operacion que quiere ejecutar el cliente
 */
int recibir_operacion(int socket_cliente);

/* 
    @NAME:  recibir_int
    @DESC:  recibe un int
 */
int recibir_int(int socket_cliente);

/* 
    @NAME:  recibir_PID
    @DESC:  recibe un PID
 */
int recibir_PID(int socket_cliente);

/* 
    @NAME:  recibir_tamanio
    @DESC:  recibe el tamanio del siguiente buffer
 */
int recibir_tamanio(int socket_cliente);

/* 
    @NAME: recibir_buffer
    @DESC: recibe un buffer dado un tamanio y el socket 
 */
void* recibir_buffer(int size, int socket_cliente);

/* 
    @NAME: recibir_mensaje
    @DESC: recibe un mensaje del socket/server
 */
char* recibir_mensaje(int socket);

/* 
    @NAME: serializar_mensaje
    @DESC: serializa un mensaje listo para enviar
 */
void* serializar_mensaje(char* mensaje);

/* 
    @NAME: serializar_paquete
    @DESC: serializa un paquete
 */
void* serializar_paquete(t_paquete* paquete, int bytes);

/* 
    @NAME: crear_buffer
    @DESC: inicializa el buffer determinado de "paquete"
 */
void crear_buffer(t_paquete* paquete);

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);

void eliminar_paquete(t_paquete* paquete);

t_paquete* crear_paquete(int codigo);

void enviar_paquete(t_paquete* paquete, int socket_cliente);

void* recibir_buffer_t(int* size, int socket);

#endif