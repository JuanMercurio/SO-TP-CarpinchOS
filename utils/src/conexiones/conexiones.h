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

#endif