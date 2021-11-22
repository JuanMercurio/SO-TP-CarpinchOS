#include "main.h"

int main(int argc, char *argv[])
{

   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);

   iniciar_logger();
   obtener_config();
   inicializar_listas_sem_io();
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

   pthread_attr_destroy(&detached2);
   pthread_attr_destroy(&detached3); 
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
   sem_destroy(cola_finalizados);
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
   int cod_op;
   bool conectado = true;
   handshake(cliente, "KERNEL");
   t_paquete_semaforo semaforo ;
   char* recibido;
   sem_kernel *sem ;
   t_paquete* paquete;

   int conexion_memoria = crear_conexion(configuracion.IP_MEMORIA, configuracion.PUERTO_MEMORIA);
   
   while (conectado)
   {
      
      log_info(logger, "Esperando mensaje");
      cod_op = recibir_operacion(cliente);
   
      log_info(logger,"Se recibio el codigo de operacion %d del cliente %d", cod_op, cliente);
      
      switch (cod_op)
      {

      case NEW_INSTANCE: 
         t_pcb *carpincho = malloc(sizeof(t_pcb)); // aca no recibe la pcb en si , recibe un paquete con datos que habra que guardar en un t_pcb luego de desserializar lo que viene
            carpincho->fd_cliente = cliente;
            carpincho->fd_memoria = conexion_memoria;
            carpincho->pid = crearID(&id_procesos);
            carpincho->estado ='N';
            char *pid = itoa(carpincho->pid);
            enviar_mensaje(cliente, pid);
            sem_wait(&mutex_cola_new);
            queue_push(cola_new, (void*) carpincho); // pensando que el proceso queda trabado en mate init hasta que sea planificado
            sem_post(&mutex_cola_new);
            log_info(logger, "Se agregó el carpincho ID: %d a la cola de new", carpincho->pid);

         // enviar_mensaje("OK", cliente);
         // aca debe enviar el ok o debe planificarlo y al llegar a exec recien destrabar el carpincho
         break;
      case INIT_SEMAFORO:// SE PUEDE MODIFICAR PARA CONFIRMAR 
              semaforo = recibir_semaforo(cliente);
               sem_kernel_init(semaforo.buffer, semaforo.valor);
               enviar_mensaje("OK", cliente);
               break;
      case IO: 
               char* io = recibir_mensaje(cliente);
               //io_kernel io_to_be_served = *(buscar_io(io, lista_io_kernel));// DIRECTAMENT, AL SER UNA LISTA GLOBAL SE PUEDE ACCEDER DESDE LA FUNJCION Y NO PASARLA TODO EL TIEMPO COMO PARAMEETRO
               carpincho->io_solicitada = io;              
               carpincho->proxima_instruccion = IO;
               sem_post(&carpincho->semaforo_evento);
         
               break;

      case SEM_WAIT: 
               recibido = recibir_mensaje(cliente);

                sem = buscar_semaforo(carpincho->semaforo_a_modificar);
                if( sem != NULL){
               strcpy(carpincho->semaforo_a_modificar ,recibido);
               sem_post(&carpincho->semaforo_evento);  
                }
                else
                {
               enviar_mensaje("FAIL", cliente);
                }
                
               break;

      case SEM_POST: recibido = recibir_mensaje(cliente);
       sem = buscar_semaforo(carpincho->semaforo_a_modificar);
                if( sem != NULL){
               sem_kernel_post(recibido);
              
               enviar_mensaje("OK", cliente);
                 }
                else
                {
               enviar_mensaje("FAIL", cliente);
                }

         break;
      case SEM_DESTROY: recibido = recibir_mensaje(cliente);
               sem_kernel_destroy(recibido);
               enviar_mensaje("OK", cliente);

         break;
      case MEMALLOC: carpincho->proxima_instruccion = MEMALLOC;
           paquete = recibir_paquete(cliente);
               
         break;
      case MEMFREE: carpincho->proxima_instruccion = MEMFREE;
              
         break;
      case MEMREAD: carpincho->proxima_instruccion = MEMREAD;
               
         break;
      case MEMWRITE:carpincho->proxima_instruccion = MEMWRITE;
               
         break;
      case MATE_CLOSE: recibir_mensaje(cliente);
               carpincho->proxima_instruccion = MATE_CLOSE;
               sem_post(&carpincho->semaforo_evento);
               enviar_mensaje("OK", cliente);
               close(cliente);
               close(conexion_memoria);
               conectado = false;
      break;
      }
   }
}

void inicializar_planificacion()
{
   pthread_t hilos_planificadores[7];
   pthread_attr_t detached3;
   pthread_attr_init(&detached3);
   pthread_attr_setdetachstate(&detached3, PTHREAD_CREATE_DETACHED);
   
   iniciar_colas();
   inicializar_semaforos();
   if(!pthread_create(&hilos_planificadores[0], &detached3, (void *) iniciar_planificador_corto_plazo, NULL)){
     looger_info(logger,"NO SE PUDO CREAR HILO PLANIFICADOR CORTO PLAZO\n");
   }
  if(!pthread_create(&hilos_planificadores[1], &detached3,  (void *)iniciar_planificador_largo_plazo, NULL)){
      looger_info(logger, "NO SE PUDO CREAR HILO PLANIFICADOR LARGO PLAZO\n");
   }if(!pthread_create(&hilos_planificadores[2], &detached3, (void *) iniciar_planificador_mediano_plazo, NULL)){
       looger_info(logger, "NO SE PUDO CREAR HILO PLANIFICADOR MEDIANO PLAZO\n");
   }if(!pthread_create(&hilos_planificadores[3], &detached3, (void *) iniciar_gestor_finalizados, NULL)){
       looger_info(logger, "NO SE PUDO CREAR HILO GESWTOR FINALIZADOS\n");
   }if(!pthread_create(&hilos_planificadores[4], &detached3, (void *) iniciar_cpu,  NULL)){
       looger_info(logger, "NO SE PUDO CREAR HILO PLANIFICADOR LARGO PLAZO\n");
   }if(!pthread_create(&hilos_planificadores[5], &detached3, (void *)deteccion_deadlock, NULL)){
      looger_info(logger, "NO SE PUDO CREAR HILO DETECCION DEADLOCK\n");
   }
   if(!pthread_create(&hilos_planificadores[6], &detached3, (void *)program_killer, NULL)){
      looger_info(logger, "NO SE PUDO CREAR HILO PARATERMINAR PROGRAMA\n");
   }
   
   logger_info(logger, "PLANIFICADORES Y DETECTOR DEADLOCK CREADOS\n");


}
void program_killer(){
   char* leido;
   looger_info(logger, "Para terminar precione cualquier tecla.\n");
   scanf(leido);
   terminar = true;
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
   
   sem_init(&cola_new_con_elementos, NULL, 0);
   sem_init(&cola_ready_con_elementos, NULL, 0);
   sem_init(&cola_suspendido_bloquedo_con_elementos, NULL, 0);
   sem_init(&cola_suspendido_listo_con_elementos, NULL, 0);
   sem_init(&cola_finalizados_con_elementos, NULL, 0);
   sem_init(cola_finalizados, NULL, 0);
   sem_init(&mutex_cola_new, NULL, 1);
   sem_init(&mutex_cola_ready, NULL, 1);
   sem_init(&mutex_cola_bloqueado_suspendido, NULL, 1);
   sem_init(&mutex_cola_listo_suspendido, NULL, 1);
   sem_init(&mutex_lista_ejecutando, NULL, 1);
   sem_init(&mutex_cola_finalizados, NULL, 1);
   sem_init(&mutex_lista_oredenada_por_algoritmo, NULL, 1);
   sem_init(&controlador_multiprogramacion, NULL, configuracion.GRADO_MULTIPROGRAMACION);


}
void inicializar_listas_sem_io(){
   lista_io_kernel = list_create();
   lista_sem_kernel = list_create();
}


