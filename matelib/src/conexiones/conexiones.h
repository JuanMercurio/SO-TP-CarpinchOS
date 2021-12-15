#ifndef _CONEXIONES_
#define _CONEXIONES_
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

/* 
    @NAME: crear_conexion
    @DESC: Se conecta a un server con IP y PUERTO
           Retorna el socket conectado
 */
int crear_conexion(char *IP, char* PUERTO);

void clientes_administrar(int servidor, void (*funcion)(void*), bool *ejecutando);
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


#endif