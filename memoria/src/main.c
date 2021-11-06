#include "main.h"

#include <conexiones/conexiones.h>

int main(int argc, char* argv[]) {

   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    
   
   obtener_config();
   iniciar_signals();
   iniciar_paginacion();
   iniciar_tlb();

   printf("El tamanio de la paguina es: %d", configuracion.TAMANIO_PAGINAS);
   administrar_clientes(configuracion.IP, configuracion.PUERTO, (void*)&atender_proceso);

   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}




