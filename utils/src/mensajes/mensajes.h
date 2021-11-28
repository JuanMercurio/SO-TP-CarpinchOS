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
    OBTENER_PAGINA,
    SOLICITUD_PAGINA,
    ESCRIBIR_PAGINA,
    INICIO_CONFIG, 
    BORRAR_PAGINA,
    BORRAR_CARPINCHO,
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

typedef struct
{
   int cod_op;
   t_buffer* buffer;
   int valor; 
   char * nombre_semaforo;
}t_paquete_semaforo;

typedef struct
{
   int cod_op;
   int value;
}t_paquete_mem;

typedef struct 
{
   int cod_op;
   int origin;
   int size;
}t_paquete_mem_read;

typedef struct{
   int cod_op;
   t_buffer *buffer;
   int dest;
   int size;
   char* stream;
}t_paquete_mem_write;
/* 
    @NAME:  handshake
    @DESC:  dado un cliente el server le manda el modulo que es 
 */
void handshake(int cliente, char* modulo);

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

void enviar_mensaje_y_cod_op(char* mensaje, int socket_cliente, int codigo_op);

void eliminar_paquete(t_paquete* paquete);

t_paquete* crear_paquete(int codigo);

void enviar_paquete(t_paquete* paquete, int socket_cliente);

void* recibir_buffer_t(int* size, int socket);

void enviar_sem_init(char* sem, int valor, int conexion, int cod_op);

void* serializar_paquete_semaforo(t_paquete_semaforo * paquete, int bytes);

void* recibir_buffer2(int* size, int socket_cliente);

int recibir_valor_sem(int conexion);

t_paquete_semaforo *recibir_semaforo(int conexion);

void enviar_mensaje_y_cod_op(char* mensaje, int socket_cliente, int codigo_op);

void enviar_mem(int conexion, int cod_op, int pid, int value);
void* serializar_paquete_mem(t_paquete_mem * paquete, int bytes);
void enviar_mem_read(int conexion, int cod_op, int origin, int size);
void enviar_mem_write(int conexion, int cod_op, void* origin, int dest, int size);
void* serializar_paquete_mem_read(t_paquete_mem_read * paquete, int bytes);
void* serializar_paquete_mem_write(t_paquete_mem_write * paquete, int bytes);
void enviar_cod_op_e_int(int conexion, int cod_op, int valor);
t_paquete_mem_read recibir_mem_read(int conexion);
int recibir_valor_int(int conexion);
t_paquete_mem_write recibir_mem_write(int cliente);

#endif