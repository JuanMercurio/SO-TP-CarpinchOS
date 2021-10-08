#include "main.h"

#include "configuracion/config.h"

#include <utils/utils.h> 
#include <conexiones/conexiones.h>

int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    

   obtener_config();

   iniciar_servidor(configuracion.IP, configuracion.PUERTO);

   /* Este modulo funciona sin hilos. Los clientes esperan a ser atendidos en cola  */

   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}