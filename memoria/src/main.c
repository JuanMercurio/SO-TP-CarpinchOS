#include "main.h"

#include "configuracion/config.h"
#include "esquema/paginacion.h"
#include "esquema/tlb.h"
#include "procesos/clientes.h"
#include "signals/signal.h"

#include <conexiones/conexiones.h>

int main(int argc, char* argv[]) {

   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    
   
   obtener_config();
   iniciar_signals();
   iniciar_paginacion();
   iniciar_tlb();

   administrar_clientes(configuracion.IP, configuracion.PUERTO, (void*)&atender_proceso);

   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}




