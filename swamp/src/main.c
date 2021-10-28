#include "main.h"

#include "configuracion/config.h"

#include <utils/utils.h> 
#include <conexiones/conexiones.h>

int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    

   /* Este modulo funciona sin hilos. Los clientes esperan a ser atendidos en cola  */
   obtener_config();

   iniciar_swamp();
   atender_clientes();
   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}

void iniciar_swamp(){
   server = iniciar_servidor(configuracion.IP, configuracion.PUERTO);
}

void atender_clientes(){ 

   // while feo
   while(1){ 
      int cliente = aceptar_cliente(server);
      printf("Se conecto alguien en el socket: %d\n", cliente);
   }
}
