#include "main.h"

#include "configuracion/config.h"
#include "esquema/paginacion.h"
#include "esquema/tlb.h"
#include "procesos/clientes.h"
#include "signals/signal.h"
#include "tests/tests.h"
#include "tests/tests_memoria_sola.h"

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

   memoria_correr();

   terminar_programa();

   return 0;
}

void memoria_correr()
{
   server = iniciar_servidor(configuracion.IP, configuracion.PUERTO);
   clientes_administrar(server, (void*)&atender_proceso, &ejecutando);
}

void terminar_programa(){
   if(swap != -1)
   sem_post(&finalizar_conexion_swap);
   eliminar_tlb_info();
   config_destroy(config);
   log_destroy(logger_memoria);
   log_destroy(logger_tlb);
   log_destroy(logger_clock);
}




