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