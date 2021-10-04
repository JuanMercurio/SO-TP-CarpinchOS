#include "main.h"

int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    
   
   obtener_config();
   iniciar_gestor_conexiones();
   inicializar_planificacion();

   terminar_programa();

   return 0;
}

void terminar_programa(){
   config_destroy(config);
}

void iniciar_gestor_conexiones(char* puerto, char* IP, t_log * logger, void (*receptor)(void*)) {
	printf("ENTRO A INCIALIZAR, GESTOR DE CONEXIONES\n");

	int servidor;
	pthread_t conexiones[40]; /// buscar como hacer dinamico
	int contador_hilos_de_conexiones = 0;
	printf("creo vector hilos y contador\n");

	servidor = iniciar_servidor(IP, puerto);
	log_info(logger, "INICIO GESTOR, listo para aceptar conexiones\n");

	while (1) {
		printf("ENTRO A ESPERAR CLIENTE\n");
		int *cliente = malloc(sizeof(int));
		*cliente = aceptar_cliente(servidor);

		printf("acepto, cliente \n");
		if (pthread_create(&conexiones[contador_hilos_de_conexiones], NULL,
				(void*) receptor, cliente) != 0) {
			log_info(logger, "NO  SE PUDO CREAR HILO PARA CONEXION %d\n",
					contador_hilos_de_conexiones);
		} else {
			printf("Se conecto un cliente!!!\n");
			contador_hilos_de_conexiones++;
// ver como cerrar los hilos de este modulo
		}
	}
}
void receptor(void* arg){
int cliente = *(int*)arg;
		free(arg);
      int cod_op;
		bool conectado = true;

      while(conectado){
			printf("esperando comando\n");
		cod_op = recibir_operacion(cliente);
		printf("CODIGO OPERACION %d de cliente %d\n", cod_op, cliente);

		switch(cod_op){

			case NUEVO_CARPINCHO:
         t_pcb * carpincho = recibir_pcb(cliente);
         if(registrar_carpincho(carpincho) != -1){
         enviar_ok(cliente);
         }
         else printf("no es posible crear estructuras para el carpincho\n");
         break;

         case IO:
         break;
}
}

void incializar_planificacion(){
   iniciar_colas();
   iniciar_planificador_corto_plazo();
   iniciar_planificador_mediano_plazo();
   iniciar_planificador_largo_plazo();
}
void iniciar_colas(){
   cola_new = queue_create();
   cola_ready = queue_create();
   suspendido_bloqueado = queue_create();
   suspendido_listo = queue_create();
}

void iniciar_planificador_corto_plazo(){
   while(1){
   sem_wait(&cola_new_con_elementos);
   sem_wait(&mutex_cola_new);
   t_pcb * carpincho = queue_pop(cola_new);
   sem_post(&mutex_cola_new);
   crear_estructuras(carpincho);
   sem_wait(&mutex_cola_ready);
   queue_push(cola_ready, carpincho);
   sem_post(&mutex_cola_ready);
   }
}
void crear_estructuras(t_pcb *carpincho){

}
void  iniciar_planificador_mediano_plazo(){

} 
void iniciar_planificador_largo_plazo(){

}
int registrar_carpincho(){

}

void recibir_pbc(){

}

void enviar_ok(){

}