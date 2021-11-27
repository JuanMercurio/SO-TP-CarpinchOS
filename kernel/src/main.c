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
 printf("arranca o no arranca?\n");
   iniciar_logger();
   printf("paso logger\n");
   obtener_config();
   printf("paso config\n");
   inicializar_semaforos();
   printf("incializo semaforos\n");
   inicializar_listas_sem_io();
   printf("inicio listas io\n");
   init_dispositivos_io();
   printf("inicio dispositivos io\n");
   inicializar_planificacion();
   administrar_clientes(configuracion.IP, configuracion.PUERTO, &receptor);

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

   if (!queue_is_empty(cola_new))
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
}

void receptor(void *arg)
{

   int cliente = *(int *)arg;
   free(arg);
   int cod_op , mem_int, aux_int;
   bool conectado = true;
   handshake(cliente, "KERNEL");
   t_pcb *carpincho;
   t_paquete_semaforo semaforo ;
   t_paquete_mem_read mem_read;
   t_paquete_mem_write mem_write;
   char* recibido;
   sem_kernel *sem ;
   
   char* io;

  
   //¿Tiene que hacer algún handshake o confirmar de alguna forma la conexión?

   while (conectado)
   {
      
      log_info(logger, "Esperando mensaje");
      cod_op = recibir_operacion(cliente);
         
      switch (cod_op)
      {

      case NEW_INSTANCE: 
            carpincho = malloc(sizeof(t_pcb)); // aca no recibe la pcb en si , recibe un paquete con datos que habra que guardar en un t_pcb luego de desserializar lo que viene
            carpincho->fd_cliente = cliente;
            carpincho->fd_memoria =  crear_conexion(configuracion.IP_MEMORIA, configuracion.PUERTO_MEMORIA);
            carpincho->pid = crearID(&id_procesos);
            carpincho->estado ='N';
            char *pid = string_itoa(carpincho->pid);
            enviar_mensaje(cliente, pid);
            sem_wait(&mutex_cola_new);
            queue_push(cola_new, (void*) carpincho); // pensando que el proceso queda trabado en mate init hasta que sea planificado
            sem_post(&mutex_cola_new);
            log_info(logger, "Se agregó el carpincho ID: %d a la cola de new", carpincho->pid);
            //FAlTA avisar a memoria de la nueva instancia


         // enviar_mensaje("OK", cliente);
         // aca debe enviar el ok o debe planificarlo y al llegar a exec recien destrabar el carpincho
            break;

      case INIT_SEMAFORO:// SE PUEDE MODIFICAR PARA CONFIRMAR  MAL
               semaforo = recibir_semaforo(cliente);
               log_info(logger, "Se recibió del carpincho %d un SEM INIT para el semáforo %s ", carpincho->pid, semaforo.buffer);
               recibido = (char*)semaforo.buffer->stream;
               sem_kernel_init(recibido, semaforo.valor);// PORBLEMA CON BUFFER
               enviar_mensaje(cliente, "OK");
               free(semaforo.buffer);
               break;
      case IO: 
               io = recibir_mensaje(cliente);
               //io_kernel io_to_be_served = *(buscar_io(io, lista_io_kernel));// DIRECTAMENT, AL SER UNA LISTA GLOBAL SE PUEDE ACCEDER DESDE LA FUNJCION Y NO PASARLA TODO EL TIEMPO COMO PARAMEETRO
               carpincho->io_solicitada = io;              
               carpincho->proxima_instruccion = IO;
               sem_post(&carpincho->semaforo_evento);
               log_info(logger, "Se recibió del carpincho %d un CALL IO para %s", carpincho->pid, io);

               break;

      case SEM_WAIT: 
               recibido = recibir_mensaje(cliente);
               sem = buscar_semaforo(carpincho->semaforo_a_modificar);
               if( sem != NULL){
                  strcpy(carpincho->semaforo_a_modificar ,recibido);
                  sem_post(&carpincho->semaforo_evento);  
                  log_info(logger, "Se recibió del carpincho %d un SEM WAIT para %s", carpincho->pid, sem->id);
               }
               else
               {
                  log_info(logger, "Se recibió del carpincho %d un SEM WAIT para un semáforo que no existe", carpincho->pid);
                  enviar_mensaje(cliente, "FAIL ");
               }
                free(recibido);
               break;

      case SEM_POST: recibido = recibir_mensaje(cliente);
               sem = buscar_semaforo(carpincho->semaforo_a_modificar);
               if( sem != NULL){
                  sem_kernel_post(recibido);
                  log_info(logger, "Se recibió del carpincho %d un SEM POST para %s", carpincho->pid, sem->id);
                  enviar_mensaje(cliente, "OK");
               }
               else
               {
                  log_info(logger, "Se recibió del carpincho %d un SEM POST para un semáforo que no existe", carpincho->pid);
                  enviar_mensaje(cliente, "FAIL");
               }
               free(recibido);
               break;
      
      case SEM_DESTROY: recibido = recibir_mensaje(cliente);
               log_info(logger, "Se recibió del carpincho %d un SEM DESTROY para %s", carpincho->pid, recibido);
               sem_kernel_destroy(recibido);
               enviar_mensaje( cliente, "OK");
               free(recibido);
               break;
      
      case MEMALLOC: carpincho->proxima_instruccion = MEMALLOC;
                mem_int = recibir_operacion(cliente);
               log_info(logger, "Se recibió del carpincho %d un MEM ALLOC", carpincho->pid);
               enviar_cod_op_e_int(carpincho->fd_memoria,MEMALLOC,mem_int);
               //ESPERAR RTA MEMORIA
               aux_int = recibir_operacion(carpincho->fd_memoria);
               if (aux_int != -1){
                   enviar_cod_op_e_int(cliente, 0, aux_int);
               }else{
                   enviar_cod_op_e_int(cliente, 0, -1);
               }
                  
               break;

      case MEMFREE: carpincho->proxima_instruccion = MEMFREE;
               mem_int = recibir_operacion(cliente);
               log_info(logger, "Se recibió del carpincho %d un MEM FREE", carpincho->pid);
               enviar_cod_op_e_int(carpincho->fd_memoria, MEMFREE,mem_int);
               aux_int = recibir_operacion(carpincho->fd_memoria);
               if (aux_int != -5){
                   enviar_cod_op_e_int(cliente, 0, 0);
               }else{
                   enviar_cod_op_e_int(cliente, 0, -5);
               }
                 
               break;

      case MEMREAD: carpincho->proxima_instruccion = MEMREAD;
               mem_read = recibir_mem_read(cliente);
               log_info(logger, "Se recibió del carpincho %d un MEM READ desde la posición %d ", carpincho->pid,mem_read.origin);
               enviar_mem_read(carpincho->fd_memoria,MEMREAD,mem_read.origin,mem_read.size);
               recibido = recibir_mensaje(carpincho->fd_memoria);
               if (strcmp(recibido,"-6") == 0){
                   enviar_mensaje(cliente,"-6");
               }else{
                   enviar_mensaje(cliente,recibido);
               }
               free(recibido);
               break;

      case MEMWRITE:carpincho->proxima_instruccion = MEMWRITE;
               mem_write = recibir_mem_write(cliente);
               log_info(logger, "Se recibió del carpincho %d un MEM write desde la posición %d con el mensjaje %s", carpincho->pid, mem_write.dest, mem_write.buffer->stream);
               enviar_mem_write(carpincho->fd_memoria, MEMWRITE, mem_write.buffer->stream,mem_write.dest, mem_write.buffer->size);
               aux_int = recibir_operacion(carpincho->fd_memoria);
               if (aux_int != -7){
                   enviar_cod_op_e_int(cliente, 0, aux_int);
               }else{
                   enviar_cod_op_e_int(cliente, 0, -7);
               }
                 
               break;

      case MATE_CLOSE: recibir_mensaje(cliente);
               log_info(logger, "Se recibió del carpincho %d un MATE CLOSE", carpincho->pid);
               carpincho->proxima_instruccion = MATE_CLOSE;
               sem_post(&carpincho->semaforo_evento);
               enviar_mensaje( cliente, "OK");
               close(cliente);
               close(carpincho->fd_memoria);
               conectado = false;
               enviar_mensaje_y_cod_op("CERRAR CONEXION", carpincho->fd_memoria, MATE_CLOSE);
      break;
      }
   }
}

void inicializar_planificacion()
{
   pthread_t hilos_planificadores;
   pthread_attr_t detached3;
   pthread_attr_init(&detached3);
   pthread_attr_setdetachstate(&detached3, PTHREAD_CREATE_DETACHED);
   
   iniciar_colas();
   log_info(logger, "INICIO COLAS PLANIFICADORAS");
   if(pthread_create(&hilos_planificadores, &detached3, (void *) iniciar_planificador_corto_plazo, NULL)!= 0){
     log_info(logger,"NO SE PUDO CREAR HILO PLANIFICADOR CORTO PLAZO\n");
   }else{
log_info(logger, "PLANIFICADORES Y DETECTOR DEADLOCK CREADOS");

   }
   
   if(pthread_create(&hilos_planificadores, &detached3,  (void *)iniciar_planificador_largo_plazo, NULL)!= 0){
      log_info(logger, "NO SE PUDO CREAR HILO PLANIFICADOR LARGO PLAZO\n");
   }else{
log_info(logger, "PLANIFICADORES Y DETECTOR DEADLOCK CREADOS");

   }
   if(pthread_create(&hilos_planificadores, &detached3, (void *) iniciar_gestor_finalizados, NULL)!= 0){
       log_info(logger, "NO SE PUDO CREAR HILO GESWTOR FINALIZADOS\n");
   }else{
log_info(logger, "PLANIFICADORES Y DETECTOR DEADLOCK CREADOS");

   
  

   }if(pthread_create(&hilos_planificadores, &detached3, (void *)&deteccion_deadlock, NULL)!= 0){
      log_info(logger, "NO SE PUDO CREAR HILO DETECCION DEADLOCK\n");
   }else{
log_info(logger, "PLANIFICADORES Y DETECTOR DEADLOCK CREADOS");

   }
   if(pthread_create(&hilos_planificadores, &detached3, (void *)&program_killer, NULL) != 0){
      log_info(logger, "NO SE PUDO CREAR HILO PARATERMINAR PROGRAMA");
   }else{
log_info(logger, "PLANIFICADORES Y DETECTOR DEADLOCK CREADOS");

   }
    if(pthread_create(&hilos_planificadores, &detached3, (void *)iniciar_cpu,  NULL)!= 0){
       log_info(logger, "NO SE PUDO CREAR HILO PLANIFICADOR LARGO PLAZO\n");
   }else{
log_info(logger, "PLANIFICADORES Y DETECTOR DEADLOCK CREADOS\n");
   }
   

}
void program_killer(){
   char* leido  = string_new();
   log_info(logger, "Para terminar precione cualquier tecla.\n");
   scanf("%s",leido);
   terminar = true;
   terminar_programa();
}

void iniciar_colas()
{
   cola_new = queue_create();
   cola_ready = queue_create();
   suspendido_bloqueado = queue_create();
   suspendido_listo = queue_create();
   cola_finalizados = queue_create();
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


}
void inicializar_listas_sem_io(){
   lista_io_kernel = list_create();
   lista_sem_kernel = list_create();
}


