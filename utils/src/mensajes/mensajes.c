#include "mensajes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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

int recibir_tamanio(int socket_cliente) {
    int size;
	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
    return size;
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

t_paquete_semaforo recibir_semaforo(int conexion){
	t_paquete_semaforo recibido;
	int size;
	char* aux;
	int desplazamiento = 0;
	void * buffer;
	buffer = recibir_buffer2(&size, conexion);
	aux = buffer; 
	strcpy(recibido.buffer, aux);
	recibido.valor = recibir_valor_sem(conexion);
	free(buffer);
}
void* recibir_buffer2(int* size, int socket_cliente)
{
	void* buffer;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size+sizeof(int));
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	return buffer;
}

int recibir_valor_sem(int conexion){
	void* valor;
	recv(conexion, valor, sizeof(int), MSG_WAITALL);
	return *(int*) valor;	
}

void enviar_mem_allocfree(int conexion, int cod_op, int pid, int value){
	t_paquete_mem_allocfree *paquete = malloc(sizeof(t_paquete_mem_allocfree));
	paquete->cod_op = cod_op;
	paquete->pid = pid,
	paquete->value = value;

	int bytes = 3* sizeof(int);
	void* a_enviar = serializar_paquete_mem_allocfree(paquete, bytes);

	send(conexion, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
}

void* serializar_paquete_mem_allocfree(t_paquete_mem_allocfree * paquete, int bytes){
	void* magico = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magico + desplazamiento, &(paquete->cod_op), sizeof(int));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->pid), sizeof(int);
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->value), sizeof(int));
	return magico;
}

void enviar_mem_rw(int conexion, int cod_op, int pid, int origin, int dest, int size){
	t_paquete_mem_rw *paquete = malloc(sizeof(t_paquete_mem_rw));
	paquete->cod_op = cod_op;
	paquete->pid = pid,
	paquete->origin = origin;
	paquete->dest = dest;
	paquete->size = size;

	int bytes = 5* sizeof(int);
	void* a_enviar = serializar_paquete_mem_allocfree(paquete, bytes);

	send(conexion, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
}

void* serializar_paquete_mem_rw(t_paquete_mem_rw * paquete, int bytes){
	void* magico = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magico + desplazamiento, &(paquete->cod_op), sizeof(int));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->pid), sizeof(int);
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->origin), sizeof(int));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->dest), sizeof(int));
	desplazamiento+= sizeof(int);

	memcpy(magico + desplazamiento, &(paquete->size), sizeof(int));	
	return magico;
}

t_paquete_mem_allocfree recibir_mem_allocfree(int conexion){
	t_paquete_mem_allocfree recibido;
	//recibido.cod_op = recibir_valor_int(conexion);
	recibido.pid = recibir_valor_int(conexion);
	recibido.value = recibir_valor_int(conexion);
} 

t_paquete_mem_rw recibir_mem_rw(int conexion){
	t_paquete_mem_rw recibido;
	//recibido.cod_op = recibir_valor_int(conexion);
	recibido.pid = recibir_valor_int(conexion);
	recibido.origin = recibir_valor_int(conexion);
	recibido.dest = recibir_valor_int(conexion);
	recibido.size = recibir_valor_int(conexion);
} 

int recibir_valor_int(int conexion){
	void* valor;
	recv(conexion, valor, sizeof(int), MSG_WAITALL);
	return *(int*) valor;	
}