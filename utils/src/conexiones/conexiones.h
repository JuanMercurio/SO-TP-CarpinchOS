#ifndef _CONEXIONES_
#define _CONEXIONES_


/* 
    @NAME: crear_conexion
    @DESC: Se conecta a un server con IP y PUERTO
           Retorna el socket conectado
 */
int crear_conexion(char *IP, char* PUERTO);

/*
    NAME: iniciar_servidor
    DESC: Inicia y pone a escuchar a un servidor en IP y PUERTO
          Retorna el socket del server
 */
int iniciar_servidor(char* IP, char *PUERTO);

/* 
    @NAME: aceptar_cliente
    @DESC: Acepta la conexione de un cliente 
           Retorna el socket para interactuar con el cliente
 */
int aceptar_cliente(int socket_servidor);

/*
    @NAME: administrar_clientes
    @DESC: Funciones que administran los hilos que se conectan a un modulo servidor
    @ARGS: IP      - ip del seridor a crear
           PUERTO  - puerto del servidor a crear
           funcion - puntero a la funcion a ejecutar en un hilo cuando se conecta un cliente
                     El hilo recibe el socket del cliente
 */
void administrar_clientes(char* IP, char* PUERTO, void (*funcion)(void*));

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
    @NAME:  recibir_operacion
    @DESC:  recibe la operacion que quiere ejecutar el cliente
 */
int recibir_operacion(int socket_cliente);

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

#endif