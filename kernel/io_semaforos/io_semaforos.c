#include "io_semaforos.h"
#include "main.h"
#include <planificacion/planificacion.h>


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
   sem_wait(&sem->mutex);
   if(sem->val == 0){
      sem->tomado_por = carpincho->pid;
   }
  if(sem->val  < 0){
    bloquear_por_semaforo( carpincho); 
    return true;
}
   //loguear-mutex_semaforos
  // free(sem); // los semaforos se liberan si se destruyen o cuando el programa termine
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
   int sem_valor_actual = obtener_valor_semaforo(carpincho->semaforo_a_modificar);
   if(sem_valor_actual -1 < 0){
      return true;
   }else
   {
      return false;
   }
   
}

void bloquear_por_semaforo(t_pcb *carpincho){
   sem_kernel *semaforo = buscar_semaforo(carpincho->semaforo_a_modificar);
   carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
   sem_wait(&semaforo->mutex_cola);
   queue_push(semaforo->bloqueados, (void*)carpincho);
   sem_post(&semaforo->mutex_cola);
}
void sem_kernel_post(char *nombre) 
{
   sem_kernel *sem = buscar_semaforo(nombre);
   sem_wait(&mutex_lista_sem_kernel);
   sem->val ++;
   //loguear
   
      if(sem->val >= 0 && !queue_is_empty(sem->bloqueados))
      {
      //Desbloqueo el proceso del sistema y lo paso a listo
     bloqueado_a_listo(sem->bloqueados, sem->mutex_cola);
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

         bloqueado_a_listo(sem->bloqueados, sem->mutex_cola);
      }
   }
 //  sem_post(&mutex_lista_sem_kernel);
   bool nombre_semaforo(void* elemento){
      return (strcmp(((sem_kernel*)elemento)->id, nombre) == 0);
   }

  list_remove_and_destroy_by_condition(lista_sem_kernel, nombre_semaforo, sem_destroyer);
}

void sem_detroyer(void* semaforo){
 sem_kernel *a_destruir  = (sem_kernel*) semaforo;
 queue_destroy(a_destruir->bloqueados); 
 sem_destroy(a_destruir->mutex);
 sem_destroy(a_destruir->mutex_cola);
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
      sem_init(nueva_io->mutex_io, NULL, 0);
      sem_init(nueva_io->cola_con_elementos, NULL, 0);
      sem_wait(&mutex_lista_io_kernel);
      list_add(lista_io_kernel, nueva_io);
      sem_wait(&mutex_lista_io_kernel);
      i++;
      printf("COLAS DISPOSITIVOS IO CREADAS\n");
     // free(nueva_io); //va acá o afuera del while? BORRAR LA TERMINAR
   }
   iniciar_hilos_gestores_de_io();
}
void inicial_hilos_gestores_de_io(){

      pthread_attr_t detached2;
      pthread_attr_init(&detached2);
      pthread_attr_setdetachstate(&detached2, PTHREAD_CREATE_DETACHED);
      int i = list_size(lista_io_kernel);
      io_kernel * io;
      while (i != 0)
      {
         pthread_t hilo2;
         io = list_get(lista_io_kernel, i);
         if (!(pthread_create(hilo2, NULL, (void *)gestor_cola_io, (void *)io)))
         {
            printf("no se pudo crear hilo gestor de cola io cerado \n");
         }
         printf("se creo GESTOR de io %s\n", io->id); // para controlar que se cree
            i--;
      }
   pthread_attr_destroy(&detached2); 
}
/*
void call_io(char *nombre, t_pcb *carpincho){
   io_kernel *io = buscar_io(nombre, lista_io_kernel);

   ejecutando_a_bloqueado(carpincho, cola); //bloqueo el carpincho ya sea que pueda ejecutar io o que tenga que esperar a que otro termine

   sem_wait(&mutex_lista_io_kernel);
   if(queue_size(io->bloqueados)==0){ //Si está vacía la lista, puede utilizar io
      queue_push(io->bloqueados, carpincho);
      realizar_io(carpincho, io->retardo);
   }
   else {
      queue_push(io->bloqueados, carpincho);
   }
   sem_post(&mutex_lista_io_kernel);
   //free(io);
}*/

/* void realizar_io(t_pcb *carpincho, io_kernel *io){

   //Lo hago esperar el tiempo que tiene la IO de retardo
   usleep(io->retardo);

   bloqueado_a_listo(carpincho, cola);
   queue_pop(io->bloqueados);
   
   //Si existe algún otro carpincho en esta io lo mando a realizar_io()
   if(queue_size(io->bloqueados)>0){
      t_pcb *proximo_carpincho_bloqueado = queue_peek(io->bloqueados);
      realizar_io(proximo_carpincho_bloqueado, io); 
   }
}*/
void gestor_cola_io(void *datos){
   io_kernel *io = (io_kernel*)datos;
   t_pcb *carpincho;
   while(1){
   sem_wait(&io->cola_con_elementos);// el posta  aeste semaforo se lo tiene que dar el procesador al bloquearlo buscando en la lista la io y su resdpectivo semaforo
   sem_wait(&io->mutex_io);
   carpincho = queue_pop(io->bloqueados);
   sem_post(&io->mutex_io);
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
}
/*
   pthread_attr_t detached;
   pthread_attr_init(&detached);
   pthread_attr_setdetachstate(&detached, PTHREAD_CREATE_DETACHED);


   while(1){
      pthread_t hilo;
      int *cliente = malloc(sizeof(int));
      *cliente= aceptar_cliente(servidor);
      pthread_create(&hilo, &detached, (void*)funcion,(void*) cliente);
   }

   pthread_attr_destroy(&detached); 
} */
