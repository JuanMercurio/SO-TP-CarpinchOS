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
      case INIT_SEMAFORO:
               t_paquete_semaforo semaforo = recibir_semaforo(cliente);
               sem_kernel_init(semaforo.buffer, semaforo.valor);
               break;
      case IO: 
               char* io = recibir_mensaje(cliente);
               //io_kernel io_to_be_served = *(buscar_io(io, lista_io_kernel));// DIRECTAMENT, AL SER UNA LISTA GLOBAL SE PUEDE ACCEDER DESDE LA FUNJCION Y NO PASARLA TODO EL TIEMPO COMO PARAMEETRO
               carpincho->io_solicitada = io;              
               carpincho->proxima_instruccion = IO;
               sem_post(&carpincho->semaforo_evento);
               break;

      case SEM_WAIT: 
               t_paquete_semaforo semaforo = recibir_semaforo(cliente);
               sem_kernel_wait(semaforo.buffer, carpincho);
               sem_kernel to_be_waited = *(buscar_semaforo(semaforo.buffer, lista_sem_kernel));
               if(to_be_waited.val == 0){/*ACA COMPRUEBA SI SE DEBE BLOQUEAR EL PROCESOS O SEGUIR A LA ESPERA DE INSTRUCCIONES */
               carpincho->proxima_instruccion = SEM_WAIT;
               sem_post(&carpincho->semaforo_evento);
               }
         break;
      case SEM_POST: carpincho->proxima_instruccion = SEM_POST;
               
         break;
      case SEM_DESTROY:

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
}
int recibir_operacion(int cliente){}

int recibir_pcb(int cliente){}

void inicializar_planificacion()
{
   iniciar_colas();
   inicializar_semaforos();
   iniciar_planificador_corto_plazo();
   iniciar_planificador_mediano_plazo();
   iniciar_planificador_largo_plazo();
   iniciar_gestor_finalizados();
   inciar_cpu();
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

//_------------------------------------------------------------poner en otro .c
sem_kernel *buscar_semaforo(char *nombre, t_list *sems)
{
   sem_kernel *sem_buscado = malloc(sizeof(sem_kernel));

   bool nombre_semaforo(void* elemento){
      return (strcmp(((sem_kernel*)elemento)->id ,nombre) == 0);
   }
   //uso la inner function porque list_find requiere un void* como condicion
   sem_buscado = list_find(sems, nombre_semaforo);
   return sem_buscado;
}

void sem_kernel_wait(char *nombre, t_pcb *carpincho)
{
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   //agregar mutex{
   sem->val =-1;
  
   //loguear-mutex_semaforos
   if (sem->val < 0)
   {
      ejecutando_a_bloqueado(carpincho);
      queue_push(sem->bloqueados, carpincho);
   }
   free(sem);
}

void sem_kernel_post(char *nombre, t_pcb *carpincho) 
{
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   sem_wait(&mutex_lista_sem_kernel);
   sem->val = +1;

   //loguear
   if (sem->val >= 0)
   { 
      if(queue_size(sem->bloqueados)>0)
      {
      //Desbloqueo el proceso del sistema y lo paso a listo
      bloqueado_a_listo(carpincho);
      queue_pop(sem->bloqueados);
      }
   }
   sem_post(&mutex_lista_sem_kernel);
   free(sem);
}

void sem_kernel_init(char* nombre, int value){
   sem_kernel *nuevo_sem = malloc(sizeof(sem_kernel));
   nuevo_sem->id=nombre;
   nuevo_sem->max_val=value;
   nuevo_sem->val=value;
   nuevo_sem->bloqueados=queue_create();
   list_add(lista_sem_kernel, nuevo_sem); //hace falta mutex acá?

}

void sem_kernel_destroy(char* nombre){
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   
   sem_wait(&mutex_lista_sem_kernel);
   int cant_bloqueados = queue_size(sem->bloqueados);
   if(cant_bloqueados>0){
      for(int i=0;cant_bloqueados; i++){
         t_pcb *carpincho_a_desbloquear = queue_peek(sem->bloqueados);
         queue_pop(sem->bloqueados);
         bloqueado_a_listo(carpincho_a_desbloquear);
      }
   }
   sem_post(&mutex_lista_sem_kernel);

   bool nombre_semaforo(void* elemento){
      return (strcmp(((sem_kernel*)elemento)->id, nombre) == 0);
   }

   //revisar si está bien planteado
   list_remove_by_condition(lista_sem_kernel, nombre_semaforo);
   //free(sem);
}

io_kernel *buscar_io(char *nombre, t_list *ios)
{
   io_kernel *io_buscada = malloc(sizeof(io_kernel));

   bool nombre_io(void* elemento){
      return (strcmp(((io_kernel*)elemento)->id ,nombre) == 0);
   }
   //uso la inner function porque list_find requiere un void* como condicion
   io_buscada = list_find(ios, nombre_io);
   return io_buscada;
}

void init_dispositivos_io(){
   int i=0;
   while (configuracion.DISPOSITIVOS_IO[i]!=NULL)
   {
      io_kernel *nueva_io = malloc(sizeof(io_kernel));

      nueva_io->id = (char*)configuracion.DISPOSITIVOS_IO[i];
      nueva_io->retardo = atoi((char*)configuracion.DURACIONES_IO[i]);
      nueva_io->bloqueados=queue_create();
      list_add(lista_io_kernel, nueva_io);
      i++;
      printf("COLAS DISPOSITIVOS IO CREADAS\n");
     // free(nueva_io); //va acá o afuera del while?
   }
}

void call_io(char *nombre, t_pcb *carpincho){
   io_kernel *io = buscar_io(nombre, lista_io_kernel);

   ejecutando_a_bloqueado(carpincho); //bloqueo el carpincho ya sea que pueda ejecutar io o que tenga que esperar a que otro termine

   sem_wait(&mutex_lista_io_kernel);
   if(queue_size(io->bloqueados)==0){ //Si está vacía la lista, puede utilizar io
      queue_push(io->bloqueados, carpincho);
      realizar_io(carpincho, io->retardo);
   }
   else {
      queue_push(io->bloqueados, carpincho);
   }
   sem_post(&mutex_lista_io_kernel);
   free(io);
}

void realizar_io(t_pcb *carpincho, io_kernel *io){

   //Lo hago esperar el tiempo que tiene la IO de retardo
   usleep(io->retardo);

   bloqueado_a_listo(carpincho);
   queue_pop(io->bloqueados);
   
   //Si existe algún otro carpincho en esta io lo mando a realizar_io()
   if(queue_size(io->bloqueados)>0){
      t_pcb *proximo_carpincho_bloqueado = queue_peek(io->bloqueados);
      realizar_io(proximo_carpincho_bloqueado, io); 
   }
}
