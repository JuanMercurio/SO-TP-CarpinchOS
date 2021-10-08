#include "main.h"

int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    
   
   obtener_config();

   administrar_clientes2(configuracion.IP, configuracion.PUERTO, (void*)atender_cliente);

   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}

void atender_cliente(void* arg){
   //int cliente = *(int*)arg;
   printf("se conecto algo\n");
   //free(arg);
}

void administrar_clientes(){

   int servidor = iniciar_servidor(configuracion.IP, configuracion.PUERTO);

   pthread_attr_t detached;
   pthread_attr_init(&detached);
   pthread_attr_setdetachstate(&detached, PTHREAD_CREATE_DETACHED);

   /* Revisar Condicion para terminar este while */
   while(1){
      pthread_t* hilo = malloc(sizeof(pthread_t));
      int cliente = aceptar_cliente(servidor);
      pthread_create(hilo, &detached, (void*)atender_cliente,(void*) &cliente);
      pthread_detach(*hilo);
   }

   pthread_attr_destroy(&detached);
}

