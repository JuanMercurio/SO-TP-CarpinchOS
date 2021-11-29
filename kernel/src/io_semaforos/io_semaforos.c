#include "io_semaforos.h"
#include "../planificacion/planificacion.h"


sem_kernel *buscar_semaforo(char *nombre)
{
   sem_kernel *sem_buscado ;
   //uso la inner function porque list_find requiere un void* como condicion
   printf("BUSCAR SEMAFORO: ENTRO\n");
  // sem_buscado = (sem_kernel*)list_find(lista_sem_kernel, nombre_semaforo((void*)nombre));
   if(sem_buscado != NULL)
   return sem_buscado;
   else
      return NULL;  
}
   bool nombre_semaforo(void* elemento){
     // printf("BUSCAR SEMAFORO: usando comparador\n");
      return (strcmp(((sem_kernel*)elemento)->id ,elemento) == 0);
   }

sem_kernel * buscar_semaforo2(char* nombre, int* pos){
   sem_kernel * comparado ;
   bool encontrado = false;
   int i = 0;
   while(  i < list_size(lista_sem_kernel) && !encontrado){
      comparado = (sem_kernel*)list_get(lista_sem_kernel, i);
      if(strcmp(comparado->id, nombre)==0){
         *pos = i;
          encontrado = true;
      }
      i++;   
   }
   if(encontrado){
   return comparado;
   }else{
      return NULL;
   }
}


bool sem_kernel_wait2( t_pcb *carpincho)
{
   int pos;
   sem_kernel *sem = buscar_semaforo2(carpincho->semaforo_a_modificar, &pos);
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
    printf("BLOQUE POR IO: carpincho %d\n", carpincho->pid);
    printf("BLOQUE POR IO: semaforo modificado %s\n", carpincho->semaforo_a_modificar);
    int *pos;
   sem_kernel *semaforo = buscar_semaforo2(carpincho->semaforo_a_modificar, pos);
 printf("BLOQUE POR IO: semaforo encontrado %s\n", semaforo->id);
   carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
   printf("BLOQUE POR IO: obtuvo tiempo \n ");
   sem_wait(&semaforo->mutex_cola);
    printf("BLOQUE POR IO: hizo wait \n ");
   queue_push(semaforo->bloqueados, (void*)carpincho);
   sem_post(&semaforo->mutex_cola);
    printf("BLOQUE POR IO: encolo en cola de semaforo \n ");
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

int sem_kernel_init(char* nombre, int value){
 printf("SEM_KERNEL_INIT: ENTRO\n");
 int *pos = malloc(sizeof(int));
   sem_kernel * comparador = buscar_semaforo2(nombre, pos);
   if(comparador != NULL){
      printf("SEM_KERNEL_INIT: encontro semaforo ya inicializado\n");
      log_info(logger, "SEMAFORO YA INICIALIZADO\n");
      return -1;
   }else{
       printf("SEM_KERNEL_INIT: creando semaforo\n");
   sem_kernel *nuevo_sem = malloc(sizeof(sem_kernel));
   nuevo_sem->id = string_duplicate(nombre);
   nuevo_sem->max_val=value;
   nuevo_sem->val=value;
   nuevo_sem->bloqueados=queue_create();
   sem_init(&nuevo_sem->mutex_cola, 0,1);
   sem_wait(&mutex_lista_sem_kernel);
   list_add(lista_sem_kernel,(void*) nuevo_sem);
   sem_kernel *aaa = (sem_kernel*)list_get(lista_sem_kernel,0);
   printf("LO QUE BUSCO---------------------------------%s\n", aaa->id);
   sem_post(&mutex_lista_sem_kernel);
   log_info(logger, "SEMAFORO %s inicializado con %d\n",(char*) nombre, value);
    printf("SEM_KERNEL_INIT: semaforo inicializado y en lista\n");
    
    printf("SEM_INIT:)))))))))))))))))))))))))))))))))pos %d\n", *pos);
    free(pos);
   return 0;
   }
}

int sem_kernel_destroy(char* nombre){// ARREGLAR
int pos;
 printf("SEM_KERNEL_DESTROY:entro\n");
   sem_kernel *sem = buscar_semaforo2(nombre, &pos);
   if(sem == NULL){
      printf("SEM_KERNEL_DESTROY: semaforo inexistente\n");
      log_info(logger, "SEMAFORO YA destruido\n");
      return -1;
   }else{

while(!queue_is_empty(sem->bloqueados)){
         bloqueado_a_listo(sem->bloqueados, &sem->mutex_cola);
         printf("SEM_KERNEL_DESTROY: carpinchos desbloqueados\n");
      }
   list_remove(lista_sem_kernel, pos);
    printf("SEM_KERNEL_DESTROY: removido de lista\n");
    sem_destroyer(sem);
    return 0;
}
}
void sem_destroyer(void* semaforo){
    printf("SEM_DESTROYER: entro\n"); 
 sem_kernel *a_destruir  = (sem_kernel*) semaforo;
 queue_destroy(a_destruir->bloqueados); 
  printf("SEM_DESTROYER: destruyo cola de bloqueados\n"); 
 sem_destroy(&a_destruir->mutex);
 sem_destroy(&a_destruir->mutex_cola);
free(a_destruir);

 printf("SEM_DESTROYER: semaforo destruido\n"); 
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
   io_kernel* comparado = NULL;
 bool  encontrado = false;
 int i = 0;
   while(i < list_size(lista_io_kernel)&& !encontrado){
   comparado = (io_kernel*) list_get(lista_io_kernel, i);
  
      printf("BUSCAR IO=entro a donde quiero %s ------- \n", comparado->id);
      if(strcmp(comparado->id, nombre)==0){
        printf("BUSCAR Io=entro a donde quiero\n");
          encontrado = true;
      }
      i++;
     
   }  printf("BUSCAR IO: encontro io %s\n", comparado->id);
   if(encontrado)
   return comparado;
   else
   {
      printf("_____________________-------va a devolver NULL\n");
      return NULL;
   }
   
}

void init_dispositivos_io(){
   int i=0;
   while (configuracion.DISPOSITIVOS_IO[i]!=NULL)
   {
      io_kernel *nueva_io = malloc(sizeof(io_kernel));
      nueva_io->id = (char*)configuracion.DISPOSITIVOS_IO[i];
      nueva_io->retardo = atoi((char*)configuracion.DURACIONES_IO[i]);
      nueva_io->bloqueados = queue_create();
   
      sem_init(&nueva_io->mutex_io, 0, 1);
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
   log_info(logger, "Creando gestor de IO");
   io_kernel *io = (io_kernel*)datos;
   t_pcb *carpincho;
   while(!terminar){
   log_info(logger,"IO %s lista para recibir carpinchos", io->id);
   sem_wait(&io->cola_con_elementos);
   carpincho = queue_pop(io->bloqueados);
   sem_post(&io->mutex_io);
   printf("GESTOR_COLA_IO: %s recibi carpincho %d\n", io->id, carpincho->pid);
   log_info(logger,"Retardo de %d para el carpincho PID %d", io->retardo, carpincho->pid);
   usleep(io->retardo);
   sem_wait(&mutex_cola_ready);
   carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS");
    printf("GESTOR_COLA_IO: enviando resultado de io..\n");
    enviar_int(carpincho->fd_cliente, 0);
   queue_push(cola_ready, (void*) carpincho);
   sem_post(&mutex_cola_ready);
   sem_post(&cola_ready_con_elementos);
    printf("GESTOR_COLA_IO: carpincho volviendo de io a ready\n");

}
}
int bloquear_por_io(t_pcb *carpincho){
   printf("BLOQUEARPO IO: entro\n");
   io_kernel *io = buscar_io(carpincho->io_solicitada);
   if(io == NULL){
       printf("BLOQUEARPO IO: devuelve -1\n");
      return -1;
   }else{
    printf("BLOQUEARPO IO: paso buscar io\n");
   carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
     printf("BLOQUEARPO IO: creo estampas y puso tiempo\n");
   sem_wait(&io->mutex_io);//time stamp
     printf("BLOQUEARPO IO: paso wait\n");
   queue_push(io->bloqueados, (void*)carpincho);
   sem_post(&io->mutex_io);
    printf("BLOQUEARPO IO: ecolo\n");
   sem_post(&io->cola_con_elementos);
   log_info(logger, "El carpincho %d está bloqueado por IO %s", carpincho->pid, io->id);
   return 0;
   }
}
