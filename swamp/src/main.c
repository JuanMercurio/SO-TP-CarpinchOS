#include "main.h"

#include "configuracion/config.h"

#include <utils/utils.h> 
#include <conexiones/conexiones.h>
#include <mensajes/mensajes.h>

int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    

   /* Este modulo funciona sin hilos. Los clientes esperan a ser atendidos en cola  */
   obtener_config();

   int server = iniciar_servidor(configuracion.IP, configuracion.PUERTO);
   while(1){
      int cliente = aceptar_cliente(server);
      // sleep();
      printf("Se conecto alguien en el socket: %d\n", cliente);
   }
   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}