#include "main.h"
#include "signals/signal.h"
#include "configuracion/config.h"
#include "esquema/tlb.h"
#include "esquema/paginacion.h"
#include "procesos/clientes.h"

#include <stdint.h>
#include <utils/utils.h> 
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

void atender_cliente(void* arg){
   int cliente = *(int*)arg;
   free(arg);

   printf("Se conecto usando el socket %d\n", cliente);
}
