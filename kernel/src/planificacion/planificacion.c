#include "planificacion.h"
#include "../io_semaforos/io_semaforos.h"

void iniciar_cpu()
{
   pthread_t cpu;
   for (int i = 0; i < configuracion.GRADO_MULTIPROCESAMIENTO; i++)
   {
      if (pthread_create(&cpu, &detached3, (void *)procesador, NULL) != 0)
      {
         log_info(logger, "NO SE PUDO CREAR HILO CPU");
      }
   }
}
void procesador()
{ // resolver la cuestion de administacion de los semaforos de los procesos
  
   while (!terminar)
   {   printf("PROCESADOR: =================================00esperando carpinchos\n");
      sem_wait(&lista_ejecutando_con_elementos);
       printf("PROCESADOR: paso wait de lista con elementos\n");
      sem_wait(&mutex_lista_oredenada_por_algoritmo);
       printf("PROCESADOR: paso wait de lista\n");
      t_pcb *carpincho = (t_pcb *)list_remove(lista_ordenada_por_algoritmo, 0);
      sem_post(&mutex_lista_oredenada_por_algoritmo);
      printf("PROCESADOR: saco de lista ejecutando ordenada a carpincho %d\n", carpincho->pid);
      carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS"); // al comenzar a ejecutar corta el eltiempo de espera
      carpincho->tiempo.tiempo_de_espera = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
      //aca el procesador lo ejecuta por lo tanto hay qu etomar el tiempo d einicio para depues tener lamdiferencia conel timep de salida
      strcpy(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin); // el momento que comeinza a ejecutar es el mismo que cuando termino la espera
      carpincho->estado = 'E';
      printf("PROCESADORRR: estado carpincho %c", carpincho->estado);
      enviar_mensaje(carpincho->fd_cliente, "OK");
       printf("PROCESADOR: ESPERANDO EVENTO BLOQUEANTE\n");
      sem_wait(&carpincho->semaforo_evento);

      switch (carpincho->proxima_instruccion)
      {
      case IO:
         if (verificar_suspension())
         { //verifiva que el grado de multiprogramacion no este copado por carpinchos bloqueados
            bloquear_por_mediano_plazo(carpincho);
            log_info(logger, "El carpincho %d fue suspendido", carpincho->pid);
         }
         bloquear_por_io(carpincho);
            log_info(logger, "El carpincho %d fue BLOQUEADO por io", carpincho->pid);
         carpinchos_bloqueados++;
         break;

      case SEM_WAIT:
         if (sem_kernel_wait2(carpincho) && verificar_suspension())
         {
            bloquear_por_mediano_plazo(carpincho);
            log_info(logger, "El carpincho %d fue suspendido", carpincho->pid);
         }
         break; /* ACA SE ATAJAN LOS ENVENTO SUQ HACEN QUE EL CARPINCHO PASE A BLOQUEADO */

      case MATE_CLOSE:
         carpincho->estado = 'F';
         printf("PROCESADOR: carpincho al asador\n");
         sem_wait(&mutex_cola_finalizados);
         queue_push(cola_finalizados, (void *)carpincho);
         sem_post(&mutex_cola_finalizados);
         sem_post(&cola_finalizados_con_elementos);
         break;
      }
   }
}
bool verificar_suspension()
{
   if (carpinchos_bloqueados == configuracion.GRADO_MULTIPROGRAMACION - 1 && list_is_empty(lista_ordenada_por_algoritmo) && !queue_is_empty(cola_new))
      return true;
   else

      return false;
}

void iniciar_planificador_corto_plazo()
{
   log_info(logger, "hola soy EL plani corto");
   while (!terminar)
   {
       printf("PCP: =======================0000 esperando carpincho\n");
      sem_wait(&cola_ready_con_elementos);
       sem_wait(&mutex_cola_ready);
      t_pcb *carpincho = (t_pcb*)queue_pop(cola_ready);
      sem_post(&mutex_cola_ready);
      printf("PLANI CORTO PLAZO. saco de cola de new al carpincho %d\n", carpincho->pid);
      printf("PCP: algoritmo elegido %s\n", configuracion.ALGORITMO_PLANIFICACION);
      if (strcmp(configuracion.ALGORITMO_PLANIFICACION, "SJF") == 0)
      {
      
         estimador(carpincho);
         sem_wait(&mutex_lista_oredenada_por_algoritmo);
         list_add_sorted(lista_ordenada_por_algoritmo, (void *)carpincho, (void *)comparador_SFJ); //falta, una vez enlistado se le cambia el estado
         sem_post(&mutex_lista_oredenada_por_algoritmo);
         sem_post(&lista_ejecutando_con_elementos);
          printf("PCP: enlisto en ordenada por algoritmo OK\n");
         int i = list_size(lista_ordenada_por_algoritmo);
         printf("PCP: list size %d\n", i);
       
      }
      else
      {
          printf("PLANI CORTO PLAZO: ordenando con HRRN\n");
         estimador_HRRN(carpincho);
               sem_wait(&mutex_lista_oredenada_por_algoritmo);
         list_add_sorted(lista_ordenada_por_algoritmo, (void *)carpincho, (void *)comparador_HRRN);
            sem_post(&mutex_lista_oredenada_por_algoritmo);
         sem_post(&lista_ejecutando_con_elementos);
         printf("PLANI CORTO PLAZO: enlisto ordenadamente\n");
      }
   }
}
void estimador(t_pcb *carpincho)

{ // de donde se saca el valor de la rafaga de cpu
   if (carpincho->tiempo.time_stamp_fin != NULL)
   { // si es null ES UN PROCESO NUEVO por lo tanto solo hace la etimacion
      carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
      printf("ESTIMADOR: carpincho que volvio de ejecutar\n ");
   }
    printf("ESTIMADOR: carpincho nuevo\n");
      printf("ESTIMADOR: ALPHA %f\n",  configuracion.ALPHA);
      printf("ESTIMADOR: estimacion antes %f\n",  carpincho->tiempo.estimacion);
      int aux = (carpincho->tiempo.estimacion * configuracion.ALPHA);
   carpincho->tiempo.estimacion = aux + (carpincho->tiempo.tiempo_ejecutado * (1 - configuracion.ALPHA));
    printf("ESTIMADOR: estimacion despues %f\n",  carpincho->tiempo.estimacion);
}

void estimador_HRRN(t_pcb *carpincho)
{ // esto la complica bastante
   estimador(carpincho);
   carpincho->tiempo.estimacion = (carpincho->tiempo.tiempo_de_espera + carpincho->tiempo.estimacion) / carpincho->tiempo.estimacion;
}
double obtener_tiempo(char *inicio, char *fin)
{ // planteo asi esta funcion para poder usarla con el teimpo de espera tambien
   int aux1, aux2;
   double dif;
   double total = 0.0;
   char **valores_inicio = string_split(inicio, ":");
   char **valores_fin = string_split(fin, ":");

   for (int i = 0; i < 4; i++)
   {

      if (strcmp(valores_inicio[i], valores_fin[i]) != 0)
      {
         aux1 = atoi(valores_inicio[i]);
         aux2 = atoi(valores_fin[i]);
         dif = aux2 - aux1;
         switch (i)
         {
         case 0:
            total += (dif * 3600); //horas
            break;
         case 1:
            total += (dif * 60); //minutos
            break;
         case 2:
            total += dif; //segundos
            break;
         case 3:
            total += (dif * 0.001); //milisegundo
            break;
         }
      }
   }
   return total;
}

bool comparador_SFJ(t_pcb *carpincho1, t_pcb *carpincho2)
{
   if (carpincho1->tiempo.estimacion < carpincho2->tiempo.estimacion)
   {
      return true;
   }
   return false;
}
bool comparador_HRRN(t_pcb *carpincho1, t_pcb *carpincho2)
{
   if (carpincho1->tiempo.estimacion > carpincho2->tiempo.estimacion)
   {
      return true;
   }
   return false;
}
void bloquear_por_mediano_plazo(t_pcb *carpincho)
{
   //AVISAR A MEMORIA QUE EL CARPINCHO ESTA SUSPENDIDO (SWAMP)

   enviar_mensaje_y_cod_op("suspenderme", carpincho->fd_memoria, SUSPENCION);// ARREGLAR
   sem_wait(&mutex_cola_bloqueado_suspendido);
   queue_push(suspendido_bloqueado, carpincho);
   sem_post(&mutex_cola_bloqueado_suspendido);
   sem_post(&cola_suspendido_bloquedo_con_elementos);
}
void planificador_mediano_plazo()
{
   t_pcb *carpincho;
   while (!terminar)
   {
      sem_wait(&cola_suspendido_bloquedo_con_elementos);
      sem_wait(&mutex_cola_bloqueado_suspendido);
      carpincho = (t_pcb *)queue_pop(suspendido_bloqueado);
      sem_post(&mutex_cola_bloqueado_suspendido);

      sem_wait(&carpincho->semaforo_fin_evento);

      sem_wait(&mutex_cola_listo_suspendido);
      queue_push(suspendido_listo, carpincho);
      sem_post(&mutex_cola_listo_suspendido);
      sem_post(&cola_suspendido_listo_con_elementos);
   }
}

void iniciar_planificador_largo_plazo()
{
   t_pcb *carpincho = NULL;
   while (1)
   {
      sem_wait(&controlador_multiprogramacion);
      sem_wait(&cola_new_con_elementos);
  printf("PLANIFICADOR LARGO PLAZO: llego un carpincho a largo plazo\n");
      if (queue_is_empty(suspendido_listo))
      { //si el mediano plazo suspendio algo tiene prioridad ese
         sem_wait(&mutex_cola_new);
         carpincho = queue_pop(cola_new);
         sem_post(&mutex_cola_new);
         printf("PLANIFICADOR LARGO PLAZO: saco de cola new\n");
         inicializar_proceso_carpincho(carpincho);
      }
      else
      {
         sem_wait(&mutex_cola_listo_suspendido);
         carpincho = queue_pop(suspendido_listo);
         enviar_mensaje_y_cod_op("sali de suspension", carpincho->fd_memoria, VUELTA_A_READY);// traer de swamp las paginasssssss
         sem_post(&mutex_cola_listo_suspendido);
      }
      sem_wait(&mutex_cola_ready);
      queue_push(cola_ready, carpincho);
      carpincho->estado = 'R';
      carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS");
      sem_post(&mutex_cola_ready);
      sem_post(&cola_ready_con_elementos);
   }
}

void inicializar_proceso_carpincho(t_pcb *carpincho)
{
   carpincho->tiempo.estimacion = configuracion.ESTIMACION_INICIAL;
   carpincho->tiempo.tiempo_ejecutado = 0;
   carpincho->tiempo.time_stamp_inicio = NULL;
   carpincho->tiempo.time_stamp_fin = NULL;
   carpincho->io_solicitada = NULL;
   carpincho->semaforo_a_modificar = NULL;
   carpincho->estado = 'N';
   sem_init(&carpincho->semaforo_evento, 0, 0);
   sem_init(&carpincho->semaforo_fin_evento, 0, 0);
}
void iniciar_gestor_finalizados()
{
   while (!terminar)
   {
      sem_wait(&cola_finalizados_con_elementos);
      printf("GESTOR DE FINALIZADOS: asando un carpincho rico....\n");
      sem_wait(&mutex_cola_finalizados);
      t_pcb *carpincho = (t_pcb*)queue_pop(cola_finalizados);
      sem_post(&mutex_cola_finalizados);
           printf("GESTOR DE FINALIZADOS: saco carpincho %d\n", carpincho->pid);
      eliminar_carpincho((void *)carpincho);
      sem_post(&controlador_multiprogramacion);
       printf("GESTOR DE FINALIZADOS: ...estuvo rico\n");
   }
}
void eliminar_carpincho(void *arg)
{ // revisar que este este borrando lo necesario y no haya free's de mas
printf("ELIMINAR CARPINCHO: entro a eliminar\n");
   t_pcb *carpincho = (t_pcb *)arg;
   if(carpincho->tiempo.time_stamp_fin != NULL){
   free(carpincho->tiempo.time_stamp_fin);
   printf("ELIMINAR CARPINCHO: libero estamps de tiempo\n");
   }
   if(carpincho->tiempo.time_stamp_inicio != NULL){
   free(carpincho->tiempo.time_stamp_inicio);
    printf("ELIMINAR CARPINCHO: libero estamps de tiempo\n");
   }
  if(carpincho->io_solicitada != NULL){
      printf("ELIMINAR CARPINCHO: libero chars\n");
   free(carpincho->io_solicitada);
   printf("ELIMINAR CARPINCHO: libero chars\n");
   }
   if(carpincho->semaforo_a_modificar != NULL){
      free(carpincho->semaforo_a_modificar);
   }
   
   sem_destroy(&carpincho->semaforo_evento);
   sem_destroy(&carpincho->semaforo_fin_evento);
   printf("ELIMINAR CARPINCHO:destruyo semaforos\n");
   close(carpincho->fd_cliente);
   close(carpincho->fd_memoria);
   printf("ELIMINAR CARPINCHO: cerro conexiones\n");
   free(carpincho);
printf("ELIMINAR CARPINCHO: elimino carpincho\n");
}

void ejecutando_a_bloqueado(t_pcb *carpincho, t_queue *cola, sem_t *mutex)
{
   carpincho->estado = 'B';
   carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);

   sem_wait(mutex);
   queue_push(cola, (void *)carpincho);
   sem_post(mutex);
}

void bloqueado_a_listo(t_queue *cola, sem_t *mutex)
{
   sem_wait(mutex);
   t_pcb *carpincho = (t_pcb*)queue_pop(cola);
   sem_post(mutex);
   carpincho->estado = 'R';
   sem_wait(&mutex_cola_ready);
   queue_push(cola_ready, (void *)carpincho);
   sem_post(&mutex_cola_ready);
   //sem_post(&carpincho->semaforo_fin_evento);
   sem_post(&cola_ready_con_elementos);
   carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS"); //Tomo el tiempo de cuando inicia la espera
}
