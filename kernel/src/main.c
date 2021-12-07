#include "main.h"
#include "io_semaforos/io_semaforos.h"
#include "planificacion/planificacion.h"
#include "deadlock/deadlock.h"
bool terminar = false;

int id_procesos = 0;
int carpinchos_bloqueados = 0;
int main(int argc, char *argv[])
{
   //solo corre si corremos el binario asi: binario test
   //tests(argc, argv[1]);

   iniciar_logger();
   printf("inicio\n");
   obtener_config(); 
    printf("inicio2\n");
   inicializar_semaforos();
   inicializar_listas_sem_io();
   init_dispositivos_io();
   printf("inicio dispositivos io\n");
   inicializar_planificacion();
   administrar_clientes(configuracion.IP, configuracion.PUERTO,(void*) &receptor);
   log_info(logger, "Kernel listo para recibir solicitudes");
   return 0;
}

void terminar_programa()
{
   config_destroy(config);
   log_destroy(logger);
   destruir_semaforos();
   destruir_colas_y_listas();
   //pthread_attr_destroy(&detached2);
   //pthread_attr_destroy(&detached3); 
}

void destruir_colas_y_listas(){
   if (!list_is_empty(lista_sem_kernel))
   {
      list_destroy_and_destroy_elements(lista_sem_kernel, (void *)sem_destroyer);
   }
   else
   {
      list_destroy(lista_sem_kernel);
   }

   list_destroy_and_destroy_elements(lista_io_kernel, (void *)io_destroyer);

   if (!queue_is_empty(cola_new))// cambiado
   {
      queue_destroy_and_destroy_elements(cola_new, (void *)eliminar_carpincho);
   }
   else
   {
      queue_destroy(cola_new);
   }
   if (!queue_is_empty(cola_ready))
   {
      queue_destroy_and_destroy_elements(cola_ready, (void *)eliminar_carpincho);
   }
   else
   {
      queue_destroy(cola_ready);
   }
   if (!queue_is_empty(suspendido_bloqueado))
   {
      queue_destroy_and_destroy_elements(suspendido_bloqueado, (void *)eliminar_carpincho);
   }
   else
   {
      queue_destroy(suspendido_bloqueado);
   }
   if (!queue_is_empty(suspendido_listo))
   {
      queue_destroy_and_destroy_elements(suspendido_listo, (void *)eliminar_carpincho);
   }
   else
   {
      queue_destroy(suspendido_listo);
   }
   if (!queue_is_empty(cola_finalizados))
   {
      queue_destroy_and_destroy_elements(cola_finalizados, (void *)eliminar_carpincho);
   }
   else
   {
      queue_destroy(cola_finalizados);
   }

   if(!list_is_empty(lista_ejecutando)){
      list_destroy_and_destroy_elements(lista_ejecutando, (void *)eliminar_carpincho);
   }
   else{
      list_destroy(lista_ejecutando);
   }
   if(!list_is_empty(lista_ordenada_por_algoritmo)){
      list_destroy_and_destroy_elements(lista_ordenada_por_algoritmo, (void *)eliminar_carpincho);
   }
   else{
      list_destroy(lista_ejecutando);
   }
   log_info(logger, "Colas, listas y sus respectivos elementos destruidos");

}

void destruir_semaforos(){
   sem_destroy(&cola_new_con_elementos);
   sem_destroy(&cola_ready_con_elementos);
   sem_destroy(&cola_suspendido_bloquedo_con_elementos);
   sem_destroy(&cola_suspendido_listo_con_elementos);
   sem_destroy(&cola_finalizados_con_elementos);
   sem_destroy(&mutex_cola_new);
   sem_destroy(&mutex_cola_ready);
   sem_destroy(&mutex_cola_bloqueado_suspendido);
   sem_destroy(&mutex_cola_listo_suspendido);
   sem_destroy(&mutex_lista_ejecutando);
   sem_destroy(&mutex_cola_finalizados);
   sem_destroy(&mutex_lista_oredenada_por_algoritmo);
   sem_destroy(&controlador_multiprogramacion);
   log_info(logger, "Semáforos destruidos");
}

void receptor(void *arg)
{
   printf("RECEPTOR E CLIENTE %d\n", *(int*)arg );
   int cliente = *(int *)arg;
   free(arg);
   int  aux_int;
   bool conectado = true;
   printf("por ahcer handshake\n");
   handshake(cliente, "KERNEL");
   t_pcb *carpincho;
   t_paquete_semaforo *semaforo ;
   char* recibido;
   sem_kernel *sem ;

   while (conectado)
   {
      
      int operacion = recibir_operacion(cliente);
      switch (operacion)
      {

      case NEW_INSTANCE: 
            log_info(logger, "\n Se recibió un NEW INSTANCE. Comienza creación del carpincho");
            carpincho = malloc(sizeof(t_pcb)); // aca no recibe la pcb en si , recibe un paquete con datos que habra que guardar en un t_pcb luego de desserializar lo que viene
            carpincho->fd_cliente = cliente;
            carpincho->pid = crearID(&id_procesos);
            carpincho->pid++;
            carpincho->estado = 'N';
/*             carpincho->fd_memoria = crear_conexion(configuracion.IP_MEMORIA, configuracion.PUERTO_MEMORIA);
            enviar_cod_op_e_int(carpincho->fd_memoria, NEW_INSTANCE_KERNEL, carpincho->pid);
            recibido = recibir_mensaje(carpincho->fd_memoria); //handshake
            aux_int = recibir_int(carpincho->fd_memoria);
            printf("recibio de memoria un %d\n", aux_int);

            if (aux_int == 0)
            { */
               printf("carpincho creado\n");
               enviar_int(cliente, carpincho->pid);
               sem_wait(&mutex_cola_new);
               queue_push(cola_new,carpincho); // pensando que el proceso queda trabado en mate init hasta que sea planificado
               sem_post(&mutex_cola_new);
               log_info(logger, "NEW INSTANCE: Se agregó el carpincho ID: %d a la cola de new", carpincho->pid);
               sem_post(&cola_new_con_elementos);
                /*  } else {
                  enviar_mensaje(cliente, "FAIL");
                } */
               break;

            case INIT_SEMAFORO: // SE PUEDE MODIFICAR PARA CONFIRMAR  MAL
               printf("MAIN:recibi un init semaforo\n");
               semaforo = recibir_semaforo(cliente);// recibe el puntero
               log_info(logger, "Se recibió del carpincho %d un SEM INIT para el semáforo %s con valor %d\n ", carpincho->pid, semaforo->nombre_semaforo, semaforo->valor);
               int resultado = sem_kernel_init(semaforo->nombre_semaforo, semaforo->valor);// usa lo que necesit   
               enviar_int(cliente, resultado);// responde peticion con ok 
               free(semaforo->nombre_semaforo);// bora lo que alloco 
               free(semaforo);
            
               break;
      case IO: 
               recibido = recibir_mensaje(cliente);
               carpincho->io_solicitada = string_duplicate(recibido);              
               carpincho->proxima_instruccion = IO;
               carpincho->io = buscar_io(carpincho->io_solicitada);
               printf("RECEPTOR: io: recibido de bloquear por io a carpincho %d\n", carpincho->pid);
               if(carpincho->io == NULL){
                  enviar_int(cliente, -1);
               }else
               {
               sem_post(&carpincho->semaforo_evento);
               enviar_int(carpincho->fd_cliente, 0);
               log_info(logger, "Se recibió del carpincho %d un CALL IO para %s", carpincho->pid, recibido);
               }free(recibido);
               break;

      case SEM_WAIT: 
               recibido = recibir_mensaje(cliente);
               carpincho->semaforo_a_modificar = string_duplicate(recibido);
               printf("RECEPTOR: semaforo %s recibido para wait de carpincho %d \n", recibido, carpincho->pid);
               int pos;
               sem = buscar_semaforo2(carpincho->semaforo_a_modificar, &pos );
                printf("RECEPTOR: ---------------------------------------\n");
               
               if( sem == NULL){
                  printf("RECEPTOR: el semaforo no se encuentra\n");
                  log_info(logger, "Se recibió del carpincho %d un SEM WAIT para un semáforo que no existe", carpincho->pid);
                  enviar_int(cliente, -1); 
               }
               else
               {printf("RECEPTOR: el semaforo encontradoe es %s\n", sem->id);
                  if(sem_kernel_wait2(carpincho)){
                  sem_post(&carpincho->semaforo_evento);  
                  enviar_int(cliente, 0); 
                  log_info(logger, "Se recibió del carpincho %d un SEM WAIT para %s y se bloqueara", carpincho->pid, sem->id);
                  }else{
                     enviar_int(cliente, 1);
                  log_info(logger, "Se recibió del carpincho %d un SEM WAIT para %s pero NO se bloqueara", carpincho->pid, sem->id);
                  }
               }
                free(recibido);
               break;

      case SEM_POST: recibido = recibir_mensaje(cliente);
               log_info(logger, "Se recibió del carpincho %d un SEM POST para %s", carpincho->pid, recibido);
               printf("MAIN:recibi un post semaforo para %s\n", recibido);
               resultado = sem_kernel_post(recibido);
               printf ("el resultado del post en el main kernel es %d \n", resultado);
               enviar_int(cliente, resultado);
               printf("mande resultado kernel post\n");               
               free(recibido);
               break;
      
      case SEM_DESTROY: recibido = recibir_mensaje(cliente);
               log_info(logger, "Se recibió del carpincho %d un SEM DESTROY para %s", carpincho->pid, recibido);
               printf("sem del destroy %s\n", recibido);
               aux_int = sem_kernel_destroy(recibido);
               printf("RECEPTOR: SEM DESTROY auxint %d\n", aux_int);
               enviar_int(cliente, aux_int);
               free(recibido);
               break;
      
      case MEMALLOC:
               aux_int = recibir_int(cliente);
               log_info(logger, "Se recibió del carpincho %d un MEM ALLOC con el tamañi %d", carpincho->pid, aux_int);
               printf("Se recibió del carpincho %d un MEM ALLOC con el tamañi %d\n", carpincho->pid, aux_int);
               enviar_cod_op_e_int(carpincho->fd_memoria,MEMALLOC, aux_int);
               aux_int = recibir_int(carpincho->fd_memoria);
               printf("la direccion logica que me llego es-- %d\n\n", aux_int);
               enviar_int(cliente, aux_int);
               // if (aux_int != -1){
               //     enviar_cod_op_e_int(cliente, 0, aux_int);
               // }else{
               //     enviar_cod_op_e_int(cliente, 0, -1);
               // }
                  
               break;

      case MEMFREE:
               aux_int = recibir_int(cliente);
               log_info(logger, "Se recibió del carpincho %d un MEM FREE con el tamanio %d", carpincho->pid, aux_int);
               printf("Se recibió del carpincho %d un MEM FREE con el tamanio %d\n", carpincho->pid, aux_int);
               enviar_cod_op_e_int(carpincho->fd_memoria, MEMFREE,aux_int);
               aux_int = recibir_int(carpincho->fd_memoria);
               enviar_int(cliente, aux_int);

               // if (aux_int != -5){
               //    //  enviar_cod_op_e_int(cliente, 0, 0);
               // }else{
               //    //  enviar_cod_op_e_int(cliente, 0, -5);
               // }
               break;

      case MEMREAD:
               aux_int = recibir_int(cliente);
               int size = recibir_int(cliente);
               log_info(logger, "Se recibió del carpincho %d un MEM READ desde la posición %d con tamañio %d", carpincho->pid, aux_int, size);
               printf("Se recibió del carpincho %d un MEM READ desde la posición %d con tamañio %d\n", carpincho->pid, aux_int, size);

               enviar_cod_op_e_int(carpincho->fd_memoria, MEMREAD, aux_int);
               enviar_int(carpincho->fd_memoria, size);

               aux_int = recibir_int(carpincho->fd_memoria);

               if (aux_int == -1) {
                  enviar_int(carpincho->fd_memoria, -1);
                  break;
               }

               int size_confimation = recibir_int(carpincho->fd_memoria);
               recibido = recibir_buffer(size_confimation,carpincho->fd_memoria);

               enviar_int(cliente, 0);
               enviar_mensaje(cliente, recibido);
               printf("MEMREAD mensaje enviado %s\n ", recibido);
               free(recibido);
               break;

      case MEMWRITE:
         recibido = recibir_mensaje(cliente);
         aux_int = recibir_int(cliente);
         printf("me llego del carpincho %s\n", recibido);

         enviar_mensaje_y_cod_op(recibido, carpincho->fd_memoria, MEMWRITE);
         enviar_int(carpincho->fd_memoria, aux_int);
         log_info(logger, "Se recibió del carpincho %d un MEM write desde la posición %d con el mensjaje %s", carpincho->pid, aux_int, recibido);

         printf("-----------------------------esperando respuesta ememoria\n" );
         aux_int = recibir_int(carpincho->fd_memoria);
         printf("aux int %d\n", aux_int );
         enviar_int(cliente, aux_int);

         free(recibido);
         break;

      case MATE_CLOSE:
               log_info(logger, "Se recibió del carpincho %d un MATE CLOSE", carpincho->pid);
               carpincho->proxima_instruccion = MATE_CLOSE;
               sem_post(&carpincho->semaforo_evento);
              // enviar_int(carpincho->fd_memoria, MATE_CLOSE);
               conectado = false;
            
              printf("RECEPTOR: #######################   terminando conexion... carpincho fue asado\n");
      break;

      default: 
      printf("codigo erroneo\n");
      conectado = false;
      break;
      }
   }
   
 /*  sem_init(&mate_close, 0, 1);
   sem_wait(&mate_close);
    printf("RECEPTOR: VA A ELIMINAR CARPINCHO A FUERA DE WHILE\n");
   //eliminar_carpincho(carpincho);
    sem_post(&controlador_multiprogramacion);
printf("RECEPTOR: saliendo\n");*/
}

void inicializar_planificacion()
{
   pthread_t hilos_planificadores;
   pthread_attr_t detached3;
   pthread_attr_init(&detached3);
   pthread_attr_setdetachstate(&detached3, PTHREAD_CREATE_DETACHED);
   
   iniciar_colas();
  log_info(logger, "Inicio colas de planificación");
    if(pthread_create(&hilos_planificadores, &detached3, (void *) planificador_mediano_plazo, NULL)!= 0){
      log_info(logger,"No se pudo crear el hilo Planificador Corto Plazo");
   }else{
      log_info(logger, "Hilo Planificador Corto Plazo creado");
   }
 
   if(pthread_create(&hilos_planificadores, &detached3,  (void *)iniciar_planificador_largo_plazo, NULL)!= 0){
      log_info(logger,"No se pudo crear el hilo Planificador Largo Plazo");
   }else{
      log_info(logger, "Hilo Planificador Largo Plazo creado");
   }

   if(pthread_create(&hilos_planificadores, &detached3, (void *) iniciar_gestor_finalizados, NULL)!= 0){
      log_info(logger,"No se pudo crear el hilo Gestor Finalizados");
   }else{
      log_info(logger, "Hilo Planificador Gestor Finalizados creado");
   }

   if(pthread_create(&hilos_planificadores, &detached3, (void *)&deteccion_deadlock, NULL)!= 0){
      log_info(logger,"No se pudo crear el hilo Detección Deadlock");
   }else{
      log_info(logger, "Hilo Detección Deadlock creado");
   }

   if(pthread_create(&hilos_planificadores, &detached3, (void *)&program_killer, NULL) != 0){
      log_info(logger,"No se pudo crear el hilo para terminar el programa");
   }else{
      log_info(logger, "Hilo para terminar el programa creado");
   }

    if(pthread_create(&hilos_planificadores, &detached3, (void *)iniciar_cpu,  NULL)!= 0){
      log_info(logger,"No se pudo crear el hilo CPU");
   }else{
      log_info(logger, "Hilo CPU creado");
   }
   

}
void program_killer(){
   char* leido  = string_new();
   log_info(logger, "Para terminar precione cualquier tecla.");
   scanf("%s",leido);
   terminar = true;
   terminar_programa();
}

void iniciar_colas()
{
   cola_new = queue_create();// cambio
   cola_ready = queue_create();
   suspendido_bloqueado = queue_create();
   suspendido_listo = queue_create();
   cola_finalizados = queue_create();
   lista_ordenada_por_algoritmo = list_create();
}
void inicializar_semaforos(){
   
   sem_init(&cola_new_con_elementos, 0, 0);
   sem_init(&cola_ready_con_elementos, 0, 0);
   sem_init(&cola_suspendido_bloquedo_con_elementos, 0, 0);
   sem_init(&cola_suspendido_listo_con_elementos, 0, 0);
   sem_init(&cola_finalizados_con_elementos, 0, 0);

   sem_init(&mutex_cola_new, 0, 1);
   sem_init(&mutex_cola_ready,0, 1);
   sem_init(&mutex_cola_bloqueado_suspendido, 0, 1);
   sem_init(&mutex_cola_listo_suspendido, 0, 1);
   sem_init(&mutex_lista_ejecutando,0, 1);
   sem_init(&mutex_cola_finalizados, 0, 1);
   sem_init(&mutex_lista_oredenada_por_algoritmo,0, 1);
   sem_init(&mutex_lista_sem_kernel,0,1);
   sem_init(&mutex_lista_io_kernel,0,1);
   sem_init(&controlador_multiprogramacion, 0, configuracion.GRADO_MULTIPROGRAMACION);
   log_info(logger, "Semáforos inicializados");


}
void inicializar_listas_sem_io(){
   lista_io_kernel = list_create();
   lista_sem_kernel = list_create();
}


