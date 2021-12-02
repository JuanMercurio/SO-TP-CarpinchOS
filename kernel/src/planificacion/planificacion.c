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
   {
      sem_wait(&lista_ejecutando_con_elementos);
      sem_wait(&mutex_lista_oredenada_por_algoritmo);
      t_pcb *carpincho = (t_pcb *)list_remove(lista_ordenada_por_algoritmo, 0);
      sem_post(&mutex_lista_oredenada_por_algoritmo);
      log_info(logger, "Carpincho %d - Comienza a ejecutar", carpincho->pid);
      printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!PROCESADOR: saco de lista ejecutando ordenada a carpincho %d\n", carpincho->pid);
      carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS"); // al comenzar a ejecutar corta el eltiempo de espera
      carpincho->tiempo.tiempo_de_espera = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
      printf("PROCESADOR:  carpincho %d TIEMPO DE ESPERA %f\n\n",carpincho->pid, carpincho->tiempo.tiempo_de_espera);
      strcpy(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
      carpincho->estado = 'E';
       printf("\nPROCESADOR: MANDO OK A CONEXION %d de carpinchooooooo %d\n\n", carpincho->fd_cliente, carpincho->pid);
      enviar_mensaje(carpincho->fd_cliente, "OK");
      printf("\nPROCESADOR: ESPERANDO EVENTO BLOQUEANTE de carpinchooooooo %d\n\n", carpincho->pid);
          printf("PROCESADOR: PROXIMA INSTRUCCION %d\n", carpincho->proxima_instruccion);
      sem_wait(&carpincho->semaforo_evento);

      switch (carpincho->proxima_instruccion)
      {
      case IO:
         if (verificar_suspension())
         { //verifiva que el grado de multiprogramacion no este copado por carpinchos bloqueados
            bloquear_por_mediano_plazo(carpincho);
            log_info(logger, "El carpincho %d fue suspendido", carpincho->pid);
         }
        //int aux = bloquear_por_io(carpincho);
            log_info(logger, "El carpincho %d fue BLOQUEADO por io", carpincho->pid);
            carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
       carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
         carpinchos_bloqueados++;
         printf("PROCESADOR: carpinchos cloqueados %d\n", carpinchos_bloqueados);
         break;

      case SEM_WAIT:
         if (verificar_suspension())
         {  
            bloquear_por_mediano_plazo(carpincho);
            log_info(logger, "El carpincho %d fue suspendido", carpincho->pid);
         }
         printf("PROCESADOR: carpincho se blqueara por semaforo\n");
         carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
       carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
       carpinchos_bloqueados++;
        printf("PROCESADOR: carpinchos cloqueados %d\n", carpinchos_bloqueados);
         break; /* ACA SE ATAJAN LOS ENVENTO SUQ HACEN QUE EL CARPINCHO PASE A BLOQUEADO */

      case MATE_CLOSE:
          printf("PROCESADOR:MATE CLOSEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
            eliminar_carpincho(carpincho);
            sem_post(&controlador_multiprogramacion);
        /* carpincho->estado = 'F';
         sem_wait(&mutex_cola_finalizados);
         queue_push(cola_finalizados, (void *)carpincho);
         sem_post(&mutex_cola_finalizados);
         sem_post(&cola_finalizados_con_elementos);
         sem_post(&mate_close);*/
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
   log_info(logger, "El planificador utiliza el algoritmo %s",configuracion.ALGORITMO_PLANIFICACION);
   while (!terminar)
   {
      sem_wait(&cola_ready_con_elementos);
      sem_wait(&mutex_cola_ready);
      t_pcb *carpincho = (t_pcb*)queue_pop(cola_ready);
      sem_post(&mutex_cola_ready);
      printf("PLANI CORTO PLAZO. saco de cola de ready al carpincho %d\n", carpincho->pid);
      if (strcmp(configuracion.ALGORITMO_PLANIFICACION, "SJF") == 0)
      {
      
         estimador(carpincho);
         sem_wait(&mutex_lista_oredenada_por_algoritmo);
        // list_add_sorted(lista_ordenada_por_algoritmo, (void *)carpincho, (void *)comparador_SFJ);
        listar_por_sjf(carpincho);
         for(int i= 0; i< list_size(lista_ordenada_por_algoritmo); i++){
            t_pcb *carpincho2= (t_pcb*) list_get(lista_ordenada_por_algoritmo, i);
            printf("lista ordenada :----------------------------------- carpincho %d, estimacion %f\n", carpincho2->pid, carpincho2->tiempo.estimacion);
         }
         sem_post(&mutex_lista_oredenada_por_algoritmo);
         sem_post(&lista_ejecutando_con_elementos);
         printf("PCP: enlisto en ordenada por algoritmo OK\n");
         int i = list_size(lista_ordenada_por_algoritmo);
         printf("PCP: list size %d\n", i);
       
      }
      else 
      {
          printf("PLANI CORTO PLAZO: ordenando con HRRN\n");
           estimador(carpincho);
         estimador_HRRN(carpincho);
               sem_wait(&mutex_lista_oredenada_por_algoritmo);
         listar_por_hrrn((void *)carpincho);
            sem_post(&mutex_lista_oredenada_por_algoritmo);
            for(int i= 0; i< list_size(lista_ordenada_por_algoritmo); i++){
          t_pcb *carpincho2= (t_pcb*) list_get(lista_ordenada_por_algoritmo, i);
         printf("lista ordenada :----------------------------------------------------------------- carpincho %d, estimacion %f\n", carpincho2->pid, carpincho2->tiempo.estimacion);
         printf("PLANI CORTO PLAZO: enlisto ordenadamente\n");
         }
         sem_post(&lista_ejecutando_con_elementos);
      }
   }
}
void listar_por_hrrn(t_pcb *carpincho){
   printf("ANTES DE ESTIMAR LISTA HRRN :----------------------------------------------------------------- carpincho %d, estimacion %f\n", carpincho->pid, carpincho->tiempo.estimacion);
   if(!list_is_empty(lista_ordenada_por_algoritmo)){
   t_pcb* comparado;
   int i = 0;
   bool ok = false;
    char* actual = temporal_get_string_time("%H:%M:%S:%MS");
    while(i < list_size(lista_ordenada_por_algoritmo)){
          comparado = (t_pcb*) list_get(lista_ordenada_por_algoritmo, i);
          printf("DESPUES DE estimar HRRN :---------------tiempo de espera antes HRRN cola carpincho %d, espera %f\n", comparado->pid, comparado->tiempo.tiempo_de_espera);
          printf("DESPUES DE estimar HRRN :----------------------------------------------------------------- carpincho %d, estimacion %f\n", comparado->pid, comparado->tiempo.estimacion);
          printf(" estampa de inicio %s\n", comparado->tiempo.time_stamp_inicio);
                    printf(" estampa de actual %s\n", actual);

      comparado->tiempo.tiempo_de_espera = obtener_tiempo(comparado->tiempo.time_stamp_inicio, actual);
      printf("DESPUES DE estimar HRRN :-------------------tiempo de espera carpincho %d, espera %f\n", comparado->pid, comparado->tiempo.tiempo_de_espera);
      estimador_HRRN(comparado);
      printf("DESPUES DE estimar HRRN :----------------------------------------------------------------- carpincho %d, estimacion %f\n", comparado->pid, comparado->tiempo.estimacion);
      i++;
    }
    i=0;
   while(i < list_size(lista_ordenada_por_algoritmo) && !ok){
      comparado = (t_pcb*) list_get(lista_ordenada_por_algoritmo, i);     
      if(carpincho->tiempo.estimacion > comparado->tiempo.estimacion){
         list_add_in_index(lista_ordenada_por_algoritmo, i, (void*)carpincho);
         ok = true;
      }
      if(carpincho->tiempo.estimacion == comparado->tiempo.estimacion){
          list_add_in_index(lista_ordenada_por_algoritmo, i+1, (void*)carpincho);
         ok = true;
      }
      i++;
   }
   if(!ok){
      list_add(lista_ordenada_por_algoritmo, (void*)carpincho);
   }
   
   }else
   {
      list_add(lista_ordenada_por_algoritmo, (void*)carpincho);
   }
 }


void listar_por_sjf(t_pcb *carpincho){
   if(!list_is_empty(lista_ordenada_por_algoritmo)){
   t_pcb* comparado;
   int i = 0;
   bool ok = false;
   while(i < list_size(lista_ordenada_por_algoritmo) && !ok){
       comparado = (t_pcb*) list_get(lista_ordenada_por_algoritmo, i);
      if(carpincho->tiempo.estimacion < comparado->tiempo.estimacion){
         list_add_in_index(lista_ordenada_por_algoritmo, i, (void*)carpincho);
         ok = true;
      }
      if(carpincho->tiempo.estimacion == comparado->tiempo.estimacion){
          list_add_in_index(lista_ordenada_por_algoritmo, i+1, (void*)carpincho);
         ok = true;
      }
      i++;
   }
   if(!ok){
      list_add(lista_ordenada_por_algoritmo, (void*)carpincho);
   }
   
   }else
   {
      list_add(lista_ordenada_por_algoritmo, (void*)carpincho);
   }
   }


void estimador(t_pcb *carpincho)

{ // de donde se saca el valor de la rafaga de cpu
  // if (carpincho->tiempo.time_stamp_fin != NULL)
   //{ // si es null ES UN PROCESO NUEVO por lo tanto solo hace la etimacion
     // carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);

      printf("\nESTIMADOR:!!!!!!!!!!!! carpincho %d  su tiempo ejecutado es: %f\n ", carpincho->pid, carpincho->tiempo.tiempo_ejecutado );
       printf("ESTIMADOR:!!!!!!!!!!!! carpincho %d  su estimacion es: %f\n ", carpincho->pid, carpincho->tiempo.estimacion);
   
      log_info(logger, "Carpincho %d - Estimación anterior sjf: %f", carpincho->pid, carpincho->tiempo.estimacion);
      
   double aux = (carpincho->tiempo.estimacion * configuracion.ALPHA);
   carpincho->tiempo.estimacion = aux + (carpincho->tiempo.tiempo_ejecutado * (1 - configuracion.ALPHA));
    log_info(logger, "Carpincho %d - Estimación actual: %f", carpincho->pid, carpincho->tiempo.estimacion);
     printf("ESTIMADOR:!!!!!!!!!!!! carpincho %d  su tiempo ejecutado es: %f\n ", carpincho->pid, carpincho->tiempo.tiempo_ejecutado );
       printf("ESTIMADOR:!!!!!!!!!!!! carpincho %d  su estimacion es: %f\n \n", carpincho->pid, carpincho->tiempo.estimacion);
   
}

void estimador_HRRN(t_pcb *carpincho)
{ // esto la complica bastante
   printf("entro a estui¿imador hrrn\n");
   printf("ESTIMADOR: hrrn carpincho %d  su tiempo de espera: %f\n ",carpincho->pid, carpincho->tiempo.tiempo_de_espera);
    printf("ESTIMADOR: hrrn carpincho %d  su estimacion es: %f\n ", carpincho->pid, carpincho->tiempo.estimacion);
     printf("ESTIMADOR:luegpo deestimador de ejecucion carpincho %d  su estimacion es: %f\n ", carpincho->pid, carpincho->tiempo.estimacion);
     if(carpincho->tiempo.estimacion != 0.000000){
   double aux= carpincho->tiempo.tiempo_de_espera + carpincho->tiempo.estimacion;
   printf(" aux %f\n", aux);

   aux = carpincho->tiempo.estimacion;
printf(" aux %f\n", aux);
   carpincho->tiempo.estimacion =(carpincho->tiempo.tiempo_de_espera + carpincho->tiempo.estimacion) / carpincho->tiempo.estimacion;   printf("paso\n");
     }
    printf("ESTIMADOR:hrrn carpincho %d  su estimacion es: %f\n ", carpincho->pid, carpincho->tiempo.estimacion);
   log_info(logger, "Carpincho %d - Estimación actual: %f", carpincho->pid, carpincho->tiempo.estimacion);
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
      log_info(logger, "Llegó un carpincho al planificador de largo plazo");
  printf("PLANIFICADOR LARGO PLAZO: llego un carpincho a largo plazo\n");
      if (queue_is_empty(suspendido_listo))
      { //si el mediano plazo suspendio algo tiene prioridad ese
         sem_wait(&mutex_cola_new);
         carpincho = queue_pop(cola_new);
         sem_post(&mutex_cola_new);
         log_info(logger, "Carpincho %d - Se saca de la cola new", carpincho->pid);
         printf("PLANIFICADOR LARGO PLAZO: saco de cola new\n");
         inicializar_proceso_carpincho(carpincho);
      }
      else
      {
         sem_wait(&mutex_cola_listo_suspendido);
         carpincho = queue_pop(suspendido_listo);
         log_info(logger, "Carpincho %d - Se saca de la cola suspendido", carpincho->pid);
         enviar_mensaje_y_cod_op("sali de suspension", carpincho->fd_memoria, VUELTA_A_READY);// traer de swamp las paginasssssss
         sem_post(&mutex_cola_listo_suspendido);
      }
      sem_wait(&mutex_cola_ready);
      queue_push(cola_ready, carpincho);
      carpincho->estado = 'R';
      carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS");
      sem_post(&mutex_cola_ready);
      sem_post(&cola_ready_con_elementos);
      printf("PLANIFICADOR LARGO PLAZO: metio a carpincho %d en READYYYYYYYYYYYYYYYYYYYYYYYYY\n", carpincho->pid);
   }
}

void inicializar_proceso_carpincho(t_pcb *carpincho)
{
   carpincho->tiempo.estimacion = configuracion.ESTIMACION_INICIAL;
   carpincho->tiempo.tiempo_ejecutado = 0.000000;
   carpincho->tiempo.tiempo_de_espera = 0.000000;
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
   t_pcb *carpincho = (t_pcb *)arg;
   if(carpincho->tiempo.time_stamp_fin != NULL){
   free(carpincho->tiempo.time_stamp_fin);
   }
   if(carpincho->tiempo.time_stamp_inicio != NULL){
   free(carpincho->tiempo.time_stamp_inicio);
   }
  if(carpincho->io_solicitada != NULL){    
   free(carpincho->io_solicitada);
   }
   if(carpincho->semaforo_a_modificar != NULL){
      free(carpincho->semaforo_a_modificar);
   }
   
   sem_destroy(&carpincho->semaforo_evento);
   sem_destroy(&carpincho->semaforo_fin_evento);
  //close(carpincho->fd_cliente);
   printf("ELIMINAR CARPINCHO: --------------------------CERRANDO CONEXION %d de carpincho %d\n", carpincho->fd_cliente, carpincho->pid);
   //close(carpincho->fd_memoria);
   log_info(logger, "Carpincho %d - Eliminado", carpincho->pid);
  // free(carpincho);
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
   log_info(logger, "Carpincho %d - Se saca de bloqueado", carpincho->pid);
   printf("BLOQUEADO_A_LISTO: desbloqueado CARPINCHO %d\n", carpincho->pid);
   //sem_post(&carpincho->semaforo_fin_evento);
   sem_post(&cola_ready_con_elementos);
   carpinchos_bloqueados --;
   carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS"); //Tomo el tiempo de cuando inicia la espera
    printf("BLOQUEADO_A_LISTO: time stamp\n");
}
