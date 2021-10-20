#ifndef _KERNEL_CONFIG_
#define _KERNEL_CONFIG_

#include <commons/config.h>
#include <commons/collections/list.h>

typedef struct {
   char* IP;
   char* PUERTO;
   char* IP_MEMORIA;
   char* PUERTO_MEMORIA;
   char* ALGORITMO_PLANIFICACION; 
   char** DISPOSITIVOS_IO;
   char** DURACIONES_IO;
   int RETARDO_CPU;
   int GRADO_MULTIPROGRAMACION;
   int GRADO_MULTIPROCESAMIENTO;
   int ALPHA;
   int ESTIMACION_INICIAL;
   int TIEMPO_DEADLOCK;

}t_cfg;

t_cfg configuracion;

t_config* config;

/*
   @NAME: obtener_config
   @DESC: obtiene toda la configuracion del archivo 
          .config en /cfg y lo pone en la variable 
          global configuracion
 */
void obtener_config();

#endif