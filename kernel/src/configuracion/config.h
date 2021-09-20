#ifndef _KERNEL_CONFIG_
#define _KERNEL_CONFIG_

#include <commons/config.h>
#include <string.h>

typedef struct {
   char* IP_MEMORIA;
   char* PUERTO_MEMORIA;
   char* ALGORITMO_PLANIFICACION; 
   char* DISPOSITIVOS_IO;
   int RETARDO_CPU;
   int GRADO_MULTIPROGRAMACION;
   int GRADO_MULTIPROCESAMIENTO;
}t_cfg;

t_cfg configuracion;
t_config* config;

void obtener_config();

#endif