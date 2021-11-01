#include "io_semaforos.h"
#include "main.h"

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
   //free(io);
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
