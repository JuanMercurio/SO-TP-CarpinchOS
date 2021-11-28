#include "mensajes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void enviar_mensaje_y_cod_op(char* mensaje, int socket_cliente, int codigo_op){
	printf("preparando mensjae\n");
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_op;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
	// eliminar_paquete(paquete);
}
void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void handshake(int cliente, char* modulo){
    enviar_mensaje(cliente, modulo);
}

void enviar_int(int socket, int codigo){

	void* buffer = malloc(sizeof(int));
	memcpy(buffer, &codigo, sizeof(int));
	send(socket, buffer, sizeof(int), 0);
	free(buffer);
}

void enviar_mensaje(int cliente, char* mensaje){
    int size = strlen(mensaje) + 1 + sizeof(int);
    void* paquete = serializar_mensaje(mensaje);
    send(cliente, paquete, size, 0);
	free(paquete);
}

int recibir_operacion(int socket_cliente) {
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}


void* recibir_buffer_t(int* size, int socket){
	void* buffer;
	recv(socket, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket, buffer, *size, MSG_WAITALL);

	return buffer;
}

int recibir_int(int socket_cliente) {
    int size;
	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
    return size;
}

int recibir_PID(int socket_cliente){
	return recibir_operacion(socket_cliente);
}

int recibir_tamanio(int socket_cliente) {
	return recibir_int(socket_cliente);
}

void* recibir_buffer(int size, int socket_cliente) {
	void * buffer = malloc(size);
	recv(socket_cliente, buffer, size, MSG_WAITALL);
	return buffer;
}

char* recibir_mensaje(int socket){
    int size = recibir_tamanio(socket);
    void* buffer = malloc(size);
    buffer = recibir_buffer(size, socket);
    return (char*)buffer;

}

void* serializar_mensaje(char* mensaje){
    int size = strlen(mensaje) + 1;
    void* buffer = malloc(sizeof(int) + size);
    int offset = 0;
    memcpy(buffer + offset, &size, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, (void*)mensaje, size);
    return buffer;
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(int codigo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
void enviar_sem_init(char* sem, int valor, int conexion, int cod_op){
	t_paquete_semaforo *paquete = malloc(sizeof(t_paquete_semaforo));
	paquete->cod_op = cod_op;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(sem) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, sem, paquete->buffer->size);
	paquete->valor = valor;

	int bytes = paquete->buffer->size + 3* sizeof(int);
	void* a_enviar = serializar_paquete_semaforo(paquete, bytes);

	send(conexion, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
	free(paquete->buffer);
	free(paquete);
}

void* serializar_paquete_semaforo(t_paquete_semaforo * paquete, int bytes){
	void* magico = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magico + desplazamiento, &(paquete->cod_op), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magico + desplazamiento, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	desplazamiento+= sizeof(int);
	memcpy(magico + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;
	memcpy(magico + desplazamiento, &(paquete->valor), sizeof(int));
	return magico;
}

t_paquete_semaforo *recibir_semaforo(int conexion){
	t_paquete_semaforo *recibido = malloc(sizeof(t_paquete_semaforo));
	printf("entro a recibir semaforo\n");
	recibido->nombre_semaforo = recibir_mensaje(conexion);
	printf("recibio mensaje con string %s\n", recibido->nombre_semaforo);
	recibido->valor = recibir_operacion(conexion);
	printf("recibiov valor %d\n", recibido->valor);

	//free(buffer);
	return recibido;
}
void* recibir_buffer2(int* size, int socket_cliente)
{
	void* buffer;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size+sizeof(int));
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	return buffer;
}

/*int recibir_valor_sem(int conexion){//arrelgar parece que se puede usar recibir operacion simplemente
	void* valor;
	recv(conexion, valor, sizeof(int), MSG_WAITALL);
	return *(int*) valor;	
} */

//----------------------------------------------------------------------

void enviar_mem_allocfree(int conexion, int cod_op, int pid, int value){
	t_paquete_mem *paquete = malloc(sizeof(t_paquete_mem));
	paquete->cod_op = cod_op;
	paquete->value = value;

	int bytes = 2* sizeof(int);
	void* a_enviar = serializar_paquete_mem(paquete, bytes);

	send(conexion, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
}

void* serializar_paquete_mem(t_paquete_mem * paquete, int bytes){
	void* magico = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magico + desplazamiento, &(paquete->cod_op), sizeof(int));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->value), sizeof(int));
	return magico;
}
void enviar_cod_op_e_int(int conexion, int cod_op, int valor){
	t_paquete_mem *paquete = malloc(sizeof(t_paquete_mem));
	paquete->cod_op = cod_op;
	paquete->value = valor;

	int bytes = 2* sizeof(int);
	void* a_enviar = serializar_paquete_mem(paquete, bytes);

	send(conexion, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
}
void enviar_mem_read(int conexion, int cod_op, int origin, int size){// probar que llegue el char dest
	t_paquete_mem_read *paquete = malloc(sizeof(t_paquete_mem_read));
	paquete->cod_op = cod_op;
	paquete->origin = origin;
	paquete->size = size;

	int bytes = 3* sizeof(int);
	void* a_enviar = serializar_paquete_mem_read(paquete, bytes);

	send(conexion, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
}

void enviar_mem_write(int conexion, int cod_op, void* origin, int dest, int size){// probar que llegue el char dest
	t_paquete_mem_write *paquete = malloc(sizeof(t_paquete_mem_write));
	paquete->cod_op = cod_op;
	
	paquete->buffer = malloc(sizeof(t_buffer));
	printf("lo que hay en DEST com void* %s\n", (char*)origin);
	printf("lo que hay en DEST com (char*) %s\n", (char*) origin);
	paquete->buffer->size = strlen(origin) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, origin, paquete->buffer->size);
	paquete->dest = dest;
	int bytes = 3* sizeof(int) + paquete->buffer->size;
	void* a_enviar = serializar_paquete_mem_write(paquete, bytes);

	send(conexion, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
	free(paquete->buffer);
	free(paquete);
}
void* serializar_paquete_mem_write(t_paquete_mem_write * paquete, int bytes){
	void* magico = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magico + desplazamiento, &(paquete->cod_op), sizeof(int));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	memcpy(magico + desplazamiento, &(paquete->dest), sizeof(int));
	desplazamiento+= sizeof(int);
	return magico;
}
void* serializar_paquete_mem_read(t_paquete_mem_read * paquete, int bytes){
	void* magico = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magico + desplazamiento, &(paquete->cod_op), sizeof(int));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->origin), sizeof(int));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->size), sizeof(int));
	desplazamiento+= sizeof(int);

	return magico;
}
/*int recibir_int_mem(int conexion){ 
	int value = recibir_operacion(conexion);
	return value;
} 
 */

t_paquete_mem_read recibir_mem_read(int conexion){
	t_paquete_mem_read recibido;
	recibido.origin = recibir_operacion(conexion);
	recibido.size = recibir_operacion(conexion);
	return recibido;
} 
t_paquete_mem_write recibir_mem_write(int cliente){
	t_paquete_mem_write recibido;
	recibido.size = recibir_operacion(cliente);
	char* buffer = recibir_buffer(recibido.size, cliente);
	strcpy(recibido.stream, buffer);
	recibido.dest = recibir_operacion(cliente);
	free(buffer);
	return recibido;
}
/*int recibir_valor_int(int conerecibir_mem_readxion){
	int valor;
	recv(conerecibir_mem_readxion, &valor, sizeof(int), MSG_WAITALL);
	return *(int*) valor;	
 */
