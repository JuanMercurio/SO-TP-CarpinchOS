#include "io_semaforos.h"
#include "../planificacion/planificacion.h"


sem_kernel *buscar_semaforo(char *nombre)
{
   sem_kernel *sem_buscado ;

   bool nombre_semaforo(void* elemento){
      return (strcmp(((sem_kernel*)elemento)->id ,nombre) == 0);
   }
   //uso la inner function porque list_find requiere un void* como condicion
   sem_buscado = (sem_kernel*)list_find(lista_sem_kernel, nombre_semaforo);
   if(sem_buscado != NULL)
   return sem_buscado;
   else
      return NULL;  
}

bool sem_kernel_wait2( t_pcb *carpincho)
{
   sem_kernel *sem = buscar_semaforo(carpincho->semaforo_a_modificar);
   
   sem_wait(&sem->mutex);
   sem->val --;
   sem_post(&sem->mutex);
   if(sem->val == 0){
      sem->tomado_por = carpincho->pid;
   }
  if(sem->val  < 0){
    bloquear_por_semaforo( carpincho); 
    return true;
}
   log_info(logger, "El carpincho %d realizo un SEM WAIT al semaforo %s", carpincho->pid, carpincho->semaforo_a_modificar);

  return false;
}
/* void sem_kernel_wait(char *nombre)
{
   sem_kernel *sem = buscar_semaforo(nombre);
   //agregar mutex{
   sem_wait(&sem->mutex);
   sem->val =-1;
   sem_post(&sem->mutex);
   //logguear
}*/

bool verificar_bloqueo_por_semaforo(t_pcb *carpincho){
   int sem_valor_actual = obtener_valor_semaforo(carpincho->semaforo_a_modificar); // falta
   if(sem_valor_actual -1 < 0){
      return true;
   }else
   {
      return false;
   }
   
}
int obtener_valor_semaforo(char* semaforo_a_modificar){
   sem_kernel * sem = buscar_semaforo(semaforo_a_modificar);
return sem->val;
}

void bloquear_por_semaforo(t_pcb *carpincho){
   sem_kernel *semaforo = buscar_semaforo(carpincho->semaforo_a_modificar);
   carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
   sem_wait(&semaforo->mutex_cola);
   queue_push(semaforo->bloqueados, (void*)carpincho);
   sem_post(&semaforo->mutex_cola);
   log_info(logger, "El carpincho %d está bloqueado por el semáforo %s", carpincho->pid, semaforo->id);
}
void sem_kernel_post(char *nombre) 
{
   sem_kernel *sem = buscar_semaforo(nombre);
   sem_wait(&mutex_lista_sem_kernel);
   sem->val ++;
   
      if(sem->val >= 0 && !queue_is_empty(sem->bloqueados))
      {
      //Desbloqueo el proceso del sistema y lo paso a listo
     bloqueado_a_listo(sem->bloqueados, &sem->mutex_cola);
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
   sem_wait(&mutex_lista_sem_kernel);
   list_add(lista_sem_kernel, nuevo_sem);
   sem_post(&mutex_lista_sem_kernel);
}

void sem_kernel_destroy(char* nombre){// ARREGLAR
   sem_kernel *sem = buscar_semaforo(nombre);
   
 //  sem_wait(&mutex_lista_sem_kernel);
   int cant_bloqueados = queue_size(sem->bloqueados);

   if(cant_bloqueados>0){
      for(int i=0;cant_bloqueados; i++){

         bloqueado_a_listo(sem->bloqueados, &sem->mutex_cola);
      }
   }
 //  sem_post(&mutex_lista_sem_kernel);
   bool nombre_semaforo(void* elemento){
      return (strcmp(((sem_kernel*)elemento)->id, nombre) == 0);
   }

  list_remove_and_destroy_by_condition(lista_sem_kernel, nombre_semaforo, (void*)sem_destroyer);
}

void sem_destroyer(void* semaforo){
 sem_kernel *a_destruir  = (sem_kernel*) semaforo;
 queue_destroy(a_destruir->bloqueados); 
 sem_destroy(&a_destruir->mutex);
 sem_destroy(&a_destruir->mutex_cola);
free(a_destruir);
}

void io_destroyer(void *arg)
{
   io_kernel *a_destruir = (io_kernel *)arg;
   if (queue_is_empty(a_destruir->bloqueados))
   {
      queue_destroy(a_destruir->bloqueados);
   }
   else
   {
      queue_destroy_and_destroy_elements(a_destruir->bloqueados, (void *)eliminar_carpincho);
   }

   sem_destroy(&a_destruir->mutex_io);
   sem_destroy(&a_destruir->cola_con_elementos);
   free(a_destruir);
}

io_kernel *buscar_io(char *nombre)
{
   io_kernel *io_buscada = malloc(sizeof(io_kernel));

   bool nombre_io(void* elemento){
      return (strcmp(((io_kernel*)elemento)->id ,nombre) == 0);
   }
   //uso la inner function porque list_find requiere un void* como condicion
   io_buscada = list_find(lista_io_kernel, nombre_io);
   return io_buscada;
}

void init_dispositivos_io(){
   int i=0;
   while (configuracion.DISPOSITIVOS_IO[i]!=NULL)
   {
      io_kernel *nueva_io = malloc(sizeof(io_kernel));
      nueva_io->id = (char*)configuracion.DISPOSITIVOS_IO[i];
      nueva_io->retardo = atoi((char*)configuracion.DURACIONES_IO[i]);
      nueva_io->bloqueados = queue_create();
   
      sem_init(&nueva_io->mutex_io, 0, 0);
      sem_init(&nueva_io->cola_con_elementos, 0, 0);
      sem_wait(&mutex_lista_io_kernel);
      list_add(lista_io_kernel, nueva_io);
      sem_post(&mutex_lista_io_kernel);
      i++;
     
   }
   log_info(logger,"Colas de dispositivos IO creadas");
   iniciar_hilos_gestores_de_io();
}
void iniciar_hilos_gestores_de_io(){

     
      pthread_attr_init(&detached2);
      pthread_attr_setdetachstate(&detached2, PTHREAD_CREATE_DETACHED);

      int i = list_size(lista_io_kernel);
      io_kernel * io;
      while (i > 0)
      {  
         pthread_t hilo2;
         io = (io_kernel*)list_get(lista_io_kernel, i-1);
         if ((pthread_create(&hilo2, NULL, (void*)gestor_cola_io, (void *)io))!=0)
         {
            log_info(logger, "No se pudo crear el hilo gestor de la IO %s", io->id);
         }else
         {  
         log_info(logger, "Se creó el hilo gestor de la IO %s", io->id); // para controlar que se cree
         }
        
            i--;
      }
   
}

void gestor_cola_io(void *datos){
   printf("acacacacacacaa\n");
   log_info(logger, "Creando gestor de IO");
   io_kernel *io = (io_kernel*)datos;
   t_pcb *carpincho;
   while(!terminar){
      log_info(logger,"IO %s lista para recibir carpinchos", io->id);
   sem_wait(&io->cola_con_elementos);// el posta  aeste semaforo se lo tiene que dar el procesador al bloquearlo buscando en la lista la io y su resdpectivo semaforo
   sem_wait(&io->mutex_io);
   carpincho = queue_pop(io->bloqueados);
   sem_post(&io->mutex_io);
   log_info(logger,"Retardo de %d para el carpincho PID %d", io->retardo, carpincho->pid);
   usleep(io->retardo);
   //enviar_mensaje("OK", carpincho->fd_cliente);
   sem_wait(&mutex_cola_ready);
   carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS");
 //  sem_post(&carpincho->semaforo_fin_evento);
   queue_push(cola_ready, (void*) carpincho);
   sem_post(&mutex_cola_ready);
   sem_post(&cola_ready_con_elementos);
}
}
void bloquear_por_io(t_pcb *carpincho){
   io_kernel *io = buscar_io(carpincho->io_solicitada);
   carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
   sem_wait(&io->mutex_io);//time stamp
   queue_push(io->bloqueados, (void*)carpincho);
   sem_post(&io->mutex_io);
   sem_post(&io->cola_con_elementos);
   log_info(logger, "El carpincho %d está bloqueado por IO %s", carpincho->pid, io->id);
}
