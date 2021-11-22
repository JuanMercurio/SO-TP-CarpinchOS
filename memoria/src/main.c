#include "main.h"

#include <conexiones/conexiones.h>

int main(int argc, char* argv[]) {

   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    
   memoria_test_solo(argc);
   
   obtener_config();
   asignacion_swap();
   iniciar_logger();
   iniciar_signals();
   iniciar_paginacion();
   iniciar_tlb();

   int server = iniciar_servidor(configuracion.IP, configuracion.PUERTO);
   swap_conexion_init(server);
   clientes_administrar(server, (void*)&atender_proceso);

   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}




