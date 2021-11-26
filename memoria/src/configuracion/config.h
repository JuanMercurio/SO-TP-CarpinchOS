#ifndef _MEMORIA_CONFIG_
#define _MEMORIA_CONFIG_

#include <commons/config.h>
#include <commons/log.h>

typedef struct {
   char* IP;
   char* PUERTO;
   int TAMANIO; 
   int TAMANIO_PAGINAS;
   char* ALGORITMO_REEMPLAZO_MMU;
   char* TIPO_ASIGNACION;
   int MARCOS_POR_CARPINCHO;
   int CANTIDAD_ENTRADAS_TLB;
   char* ALGORITMO_REEMPLAZO_TLB;
   int RETARDO_ACIERTO_TLB;
   int RETARDO_FALLO_TLB;
   char* PATH_DUMP_TLB;
}t_cfg;

extern pthread_mutex_t mutex_log;
extern t_cfg configuracion;
extern t_config* config;
extern t_log* logger;

/*
   @NAME: obtener_config
   @DESC: obtiene toda la configuracion del archivo 
          .config en /cfg y lo pone en la variable 
          global configuracion
 */
void obtener_config();

/*
   @NAME: iniciar_logger
   @DESC: inicia el loger principal de toda la memoria
 */
void iniciar_logger();

/*
   @NAME: obtener_config
   @DESC: logea un mensaje a cfg/memoria.log
          Es thread safe
 */
void loggear_mensaje(char* mensaje);

#endif
