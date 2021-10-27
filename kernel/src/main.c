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
         if (registrar_carpincho(carpincho) != -1)// comprobaciion por la memoria????
         {
            printf("NO SE PUEDE ALOJAR CARPINCHO\n");
         }else{
            carpincho->pid = crearID(&id_procesos);
            char *pid = itoa(carpincho->pid);
            enviar_mensaje(cliente, pid);
            sem_wait(&mutex_cola_new);
            queue_push(cola_new, (void*) carpincho); // pensando que el proceso queda trabado en mate init hasta que sea planificado
            sem_post(&mutex_cola_new);
           }// enviar_mensaje("OK", cliente);
         // aca debe enviar el ok o debe planificarlo y al llegar a exec recien destrabar el carpincho
         break;

      case IO: carpincho->proxima_instruccion = IO;
               sem_post(&carpincho->semaforo_evento);
               break;
      case SEM_WAIT: carpincho->proxima_instruccion = SEM_WAIT;
               sem_post(&carpincho->semaforo_evento);
         break;
      case SEM_POST: carpincho->proxima_instruccion = SEM_POST;
               sem_post(&carpincho->semaforo_evento);
         break;
      case SEM_DESTROY: carpincho->proxima_instruccion = SEM_DESTROY;
               sem_post(&carpincho->semaforo_evento);
         break;
      case MEMALLOC: carpincho->proxima_instruccion = MEMALLOC;
               sem_post(&carpincho->semaforo_evento);
         break;
      case MEMFREE: carpincho->proxima_instruccion = MEMFREE;
               sem_post(&carpincho->semaforo_evento);
         break;
      case MEMREAD: carpincho->proxima_instruccion = MEMREAD;
               sem_post(&carpincho->semaforo_evento);
         break;
      case MEMWRITE:carpincho->proxima_instruccion = MEMWRITE;
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


void crear_estructuras(t_pcb *carpincho)
{
}

int registrar_carpincho()
{
}

void recibir_pbc()
{
}

void enviar_ok()
{
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
      return ((sem_kernel*)elemento)->id == nombre;
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
      //bloquear proceso
   }
   free(sem);
}

void sem_kernel_post(char *nombre) 
{
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   sem_wait(&mutex_lista_sem_kernel);
   sem->val = +1;

   //loguear
   if (sem->val >= 0)
   { 
      if(queue_size(sem->bloqueados)>0)
      {
      //desbloquear proceso
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
   free(nuevo_sem);
}

void sem_kernel_destroy(char* nombre){
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   
   sem_wait(&mutex_lista_sem_kernel);
   int bloqueados = queue_size(sem->bloqueados);
   if(bloqueados>0){
      for(int i=0;bloqueados; i++){
         //desbloquear proceso
      }
   }
   sem_post(&mutex_lista_sem_kernel);

   bool nombre_semaforo(void* elemento){
      return ((sem_kernel*)elemento)->id == nombre;
   }

   //revisar si está bien planteado
   list_remove_by_condition(lista_sem_kernel, nombre_semaforo);
}

io_kernel *buscar_io(char *nombre, t_list *ios)
{
   io_kernel *io_buscada = malloc(sizeof(io_kernel));

   bool nombre_io(void* elemento){
      return ((io_kernel*)elemento)->id == nombre;
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
     // free(nueva_io); //va acá o afuera del while?
   }
}

void call_io(char *nombre, t_pcb *carpincho){
   io_kernel *io = buscar_io(nombre, lista_io_kernel);

   sem_wait(&mutex_lista_io_kernel);
   if(queue_size(io->bloqueados)==0){
      queue_push(io->bloqueados, carpincho);
      bloquear_proceso_io(carpincho, io->retardo);
   }
   else {
      queue_push(io->bloqueados, carpincho);
   }
   sem_post(&mutex_lista_io_kernel);
   free(io);
}

void bloquear_proceso_io(t_pcb *carpincho, io_kernel *io){
   //pasar el carpincho a la lista de bloqueados del planificador
   
   usleep(io->retardo);

   //desbloquear proceso en el planificador y de la lista de io
   
   if(queue_size(io->bloqueados)>0){
      //bloquear_proceso_io() para el siguiente en la lista de bloqueados
   }
}