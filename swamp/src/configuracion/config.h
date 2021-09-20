#ifndef _KERNEL_CONFIG_
#define _KERNEL_CONFIG_

#include <commons/config.h>
#include <string.h>

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

void obtener_config();

#endif