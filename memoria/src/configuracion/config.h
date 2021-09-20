#ifndef _MEMORIA_CONFIG_
#define _MEMORIA_CONFIG_

#include <commons/config.h>
#include <string.h>

typedef struct {
   char* IP;
   char* PUERTO;
   char* TIPO_ASIGNACION;
   char* ALGORITMO_REEMPLAZO_MMU;
   char* ALGORITMO_REEMPLAZO_TLB;
   int TAMANIO; 
   int CANTIDAD_ENTRADAS_TLB;
   int RETARDO_ACIERTO_TLB;
   int RETARDO_FALLO_TLB;
}t_cfg;

t_cfg configuracion;
t_config* config;

void obtener_config();

#endif