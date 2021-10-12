#include "main.h"

#include "configuracion/config.h"

#include <pthread.h>
#include <utils/utils.h> 
#include <conexiones/conexiones.h>
#include <signal.h>
#include <unistd.h>


void handle_sigint(int sig){
   printf("HAY QUE CODEAR ESTE COMPORTAMIENTO, IMPRIMIR METRICAS Y TERMINAR signal: %d\n", sig);
   //terminar_programa();
}
void handle_sigusr2(int sig){
   printf("LIMPIAR TLB signal: %d\n", sig);
}

void  handle_sigusr1(int sig){
   printf("HACER DUMP DE TLB signal: %d\n", sig);
}



int main(int argc, char* argv[]) {

   struct sigaction sa2 = {0};
   struct sigaction sa1 = {0};

   struct sigaction sa;
   sa.sa_handler = &handle_sigint;
   sa.sa_flags = SA_RESTART;
   sigaction(SIGINT, &sa, NULL);

   sa1.sa_flags = SA_RESTART;
   sa1.sa_handler = &handle_sigusr1;
   sigaction(SIGUSR1, &sa1, NULL);

   sa2.sa_flags = SA_RESTART;
   sa2.sa_handler = &handle_sigusr2;
   sigaction(SIGUSR2, &sa2, NULL);
   

   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    
   
   obtener_config();
   int pid = getppid();
   printf("PID: %d\n", pid);
  administrar_clientes(configuracion.IP, configuracion.PUERTO, &atender_cliente);


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


