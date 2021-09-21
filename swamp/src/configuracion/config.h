#ifndef _SWAMP_CONFIG_
#define _SWAMP_CONFIG_

#include <commons/config.h>

typedef struct {
   char* IP;
   char* PUERTO;
   int TAMANIO_SWAP;
   int TAMANIO_PAGINA;
   char* ARCHIVOS_SWAP;
   int MARCOS_MAXIMOS;
   int RETARDO_SWAP;
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