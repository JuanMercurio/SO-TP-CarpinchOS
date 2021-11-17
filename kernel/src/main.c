#include "main.h"

int main(int argc, char *argv[])
{

   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);

   obtener_config();

   administrar_clientes(configuracion.IP, configuracion.PUERTO, &receptor);

   inicializar_planificacion();
   inicializar_listas_sem_io();

   terminar_programa();

   return 0;
}

void terminar_programa()
{
   config_destroy(config);
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

   while (conectado)
   {
      printf("esperando comando\n");
      cod_op = recibir_operacion(cliente);
      printf("CODIGO OPERACION %d de cliente %d\n", cod_op, cliente);

      switch (cod_op)
      {

      case NEW_INSTANCE: 
         t_pcb *carpincho = malloc(sizeof(t_pcb)); // aca no recibe la pcb en si , recibe un paquete con datos que habra que guardar en un t_pcb luego de desserializar lo que viene
         carpincho->fd_cliente = cliente;
       
            carpincho->pid = crearID(&id_procesos);
            carpincho->estado ='N';
            char *pid = itoa(carpincho->pid);
            enviar_mensaje(cliente, pid);
            sem_wait(&mutex_cola_new);
            queue_push(cola_new, (void*) carpincho); // pensando que el proceso queda trabado en mate init hasta que sea planificado
            sem_post(&mutex_cola_new);

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
               conectado = false;
      break;
      }
   }
}

void inicializar_proceso_carpincho(t_pcb *carpincho)
{
   carpincho->tiempo.estimacion = configuracion.ESTIMACION_INICIAL;
   carpincho->tiempo.tiempo_ejecutado = 0;
   carpincho->tiempo.time_stamp_inicio = NULL;
   carpincho->tiempo.time_stamp_fin = NULL;
   carpincho->estado = 'N';
   sem_init(&carpincho->semaforo_evento, NULL, 0);
   sem_init(&carpincho->semaforo_fin_evento, NULL, 0);
   list_create(carpincho->semaforos_waiteados);
}


void inicializar_planificacion()
{
   iniciar_colas();
   inicializar_semaforos();
   iniciar_planificador_corto_plazo();
   iniciar_planificador_largo_plazo();
   iniciar_planificador_mediano_plazo();
   iniciar_gestor_finalizados();
   inciar_cpu();
   algoritmo_deteccion_deadlock();
}
void iniciar_colas()
{
   cola_new = queue_create();
   cola_ready = queue_create();
   suspendido_bloqueado = queue_create();
   suspendido_listo = queue_create();
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

void algoritmo_deteccion_deadlock(){
   pthread_t hilo;
   pthread_create(&hilo, NULL, (void*)deteccion_deadlock, NULL);
}


