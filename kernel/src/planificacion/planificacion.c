#include "planificacion.h"
#include "../io_semaforos/io_semaforos.h"

void iniciar_cpu()
{
   pthread_t cpu;
   for (int i = 0; i < configuracion.GRADO_MULTIPROCESAMIENTO; i++)
   {
      if (pthread_create(&cpu, &detached2, (void*)procesador, NULL) != 0)
      {
         log_info(logger, "NO SE PUDO CREAR HILO CPU");
      }
   }
}
void procesador()
{ t_pcb *carpincho;
   while(!terminar)
   {  sem_wait(&lista_ejecutando_con_elementos);
      if(!terminar){
      //rintf("paso a procesador\n");
      sem_wait(&mutex_lista_oredenada_por_algoritmo);
      carpincho = (t_pcb *)list_remove(lista_ordenada_por_algoritmo, 0);
      sem_post(&mutex_lista_oredenada_por_algoritmo);
      printf("PROCESADOR: saco carpincho %d de lista orednada\n",carpincho->pid);
      /*       free(carpincho->tiempo.time_stamp_inicio);
 */    //  carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS"); // TIEMPO DE ESPERA
      gettimeofday(&carpincho->tiempo_fin, NULL);
      carpincho->tiempo.tiempo_de_espera = obtener_tiempo(carpincho);
      log_info(logger, "Carpincho %d - Comienza a ejecutar", carpincho->pid);
      printf("PROCESADOR: carpincho %d va a ejecutar\n",carpincho->pid);
      carpincho->tiempo_inicio = carpincho->tiempo_fin;
      carpincho->estado = 'E';
       printf("PROCESADOR: paso tiempo y estado\n");
      enviar_mensaje(carpincho->fd_cliente, "OK");
       printf("PROCESADOR: carpincho %d envio ok \n",carpincho->pid);
      printf("\nPROCESADOR: ESPERANDO EVENTO BLOQUEANTE de carpinchooooooo %d\n\n", carpincho->pid);
     // printf("puntero a semaforo envento %d-------------------------------------------------carpincho %d\n", &carpincho->semaforo_evento, carpincho->pid);
      sem_wait(&carpincho->semaforo_evento);
       printf("PROCESADOR: carpincho %d paso el wait \n",carpincho->pid);
      switch (carpincho->proxima_instruccion)
      {
      case IO:
         if (verificar_suspension())
         { //printf("BLOQUEAR POR SUSPENDIDO CARPINCHO %d\n", carpincho->pid);
            bloquear_por_mediano_plazo(carpincho);
            log_info(logger, "---------------El carpincho %d fue suspendido", carpincho->pid);
         }else
         {
            carpinchos_bloqueados++;
         }
         sem_wait(&carpincho->io->mutex_io);
         queue_push(carpincho->io->bloqueados, (void*)carpincho);
         sem_post(&carpincho->io->mutex_io);
         sem_post(&carpincho->io->cola_con_elementos);
         log_info(logger, "---------------El carpincho %d está bloqueado por IO %s", carpincho->pid, carpincho->io->id);
         //carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");//TIEMPO EJECUTADO FIN
         gettimeofday(&carpincho->tiempo_fin, NULL);
         carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho);
         log_info(logger,"Carpinchos bloqueados: %d", carpinchos_bloqueados);
         //printf("PROCESADOR: carpinchos bloqueados--------------------------- %d\n", carpinchos_bloqueados);
         break;

      case SEM_WAIT: printf("PROCESADOR:sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss\n");
         if (verificar_suspension())
         {  //printf("BLOQUEAR POR SUSPENDIDO CARPINCHO %d\n", carpincho->pid);
            bloquear_por_mediano_plazo(carpincho);
            log_info(logger, "---------------El carpincho %d fue suspendido", carpincho->pid);
         }
         gettimeofday(&carpincho->tiempo_fin, NULL);
          printf("PROCESADOR:1\n");
         carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho);
          printf("PROCESADOR:1\n");
         carpinchos_bloqueados++;
          printf("PROCESADOR:1\n");
         printf("PROCESADOR: carpinchos bloqueados %d waiiiiitttttttttttttttttttttttttttttttttttttttttt\n", carpinchos_bloqueados);
         log_info(logger,"Carpinchos bloqueados: %d", carpinchos_bloqueados);         
         break; /* ACA SE ATAJAN LOS ENVENTO SUQ HACEN QUE EL CARPINCHO PASE A BLOQUEADO */

      case MATE_CLOSE:
      eliminar_carpincho(carpincho);
         break;
      }
      }
      
   }//printf("termino procesador\n");
}

bool verificar_suspension()
{

   if (carpinchos_bloqueados >= configuracion.GRADO_MULTIPROGRAMACION - 1 && list_is_empty(lista_ordenada_por_algoritmo) && !queue_is_empty(cola_new))//cambiado
      return true;
   else{
   log_info(logger, "No se deben suspender carpinchos. Carpinchos bloqueados: %d", carpinchos_bloqueados);
      return false;
   }
}
void iniciar_planificador_corto_plazo(t_pcb* carpincho)
{  //printf("entro plani corto\n");
   sem_wait(&mutex_lista_oredenada_por_algoritmo);
   carpincho->estado = 'R';
 // carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS");//INICIO TIEMPO DE ESPERA
 gettimeofday(&carpincho->tiempo_inicio, NULL);
     if (strcmp(configuracion.ALGORITMO_PLANIFICACION, "SJF") == 0)
      {   estimador(carpincho);
         listar_por_sjf(carpincho);
       /*   for(int i= 0; i< list_size(lista_ordenada_por_algoritmo); i++){
         t_pcb* carpincho2= (t_pcb*) list_get(lista_ordenada_por_algoritmo, i);
         //printf("\nlista ordenada SJF :------------------------------------------------ carpincho %d, estimacion %f\n", carpincho2->pid, carpincho2->tiempo.estimacion);
         } */
      }
      else
      {  estimador(carpincho);
          estimador_HRRN(carpincho);
         listar_por_hrrn((void *)carpincho);
     /*     for (int i = 0; i < list_size(lista_ordenada_por_algoritmo); i++)
         {
            t_pcb *carpincho2 = (t_pcb *)list_get(lista_ordenada_por_algoritmo, i);
            //printf("\nlista ordenada HRRN :----------------------------------------------------------------- carpincho %d, estimacion %f\n", carpincho2->pid, carpincho2->tiempo.estimacion);
         }  */

      } //sem_post(&mutex_lista_oredenada_por_algoritmo);
       sem_post(&mutex_lista_oredenada_por_algoritmo);
        sem_post(&lista_ejecutando_con_elementos); 
}





void listar_por_hrrn(t_pcb *carpincho)
{
   if (!list_is_empty(lista_ordenada_por_algoritmo))
   {
      t_pcb *comparado;
      int i = 0;
      bool ok = false;
      while(i < list_size(lista_ordenada_por_algoritmo))
      {
         comparado = (t_pcb *)list_get(lista_ordenada_por_algoritmo, i);
         //mostrar_tiempos(comparado);
         mostrar_tiempos_log(comparado);
         comparado->tiempo.tiempo_de_espera = comparado->tiempo.tiempo_de_espera + carpincho->tiempo.estimacion;
         estimador_HRRN(comparado);
         //mostrar_tiempos(comparado); 
         mostrar_tiempos_log(comparado);
         i++;
      }
      i = 0;
      while (i < list_size(lista_ordenada_por_algoritmo) && !ok)
      {
         comparado = (t_pcb *)list_get(lista_ordenada_por_algoritmo, i);
         if (carpincho->tiempo.estimacion > comparado->tiempo.estimacion)
         {
            list_add_in_index(lista_ordenada_por_algoritmo, i, (void *)carpincho);
            ok = true;
         }
         if (carpincho->tiempo.estimacion == comparado->tiempo.estimacion)
         {
            list_add_in_index(lista_ordenada_por_algoritmo, i + 1, (void *)carpincho);
            ok = true;
         }
         i++;
      }
      if (!ok)
      {
         list_add(lista_ordenada_por_algoritmo, (void *)carpincho);
      }
   }else{
       list_add(lista_ordenada_por_algoritmo, (void *)carpincho);
   }
}
void mostrar_tiempos(t_pcb *carpincho)
{
   printf("----------------------------\n");
   printf("carpincho %d\n" ,carpincho->pid);
   printf("tiempo de espera: %f\n", carpincho->tiempo.tiempo_de_espera);
   printf("tiempo de ejecutado: %f\n", carpincho->tiempo.tiempo_ejecutado);
   printf("estimacion: %f\n", carpincho->tiempo.estimacion);
  // printf("tiempo que entro: %s\n", carpincho->tiempo.time_stamp_inicio);
  // printf("tiempo que salio de lo ultimo: %s\n", carpincho->tiempo.time_stamp_fin);
   printf("----------------------------\n");
}

void mostrar_tiempos_log(t_pcb *carpincho){
   log_info(logger,"--------------Carpincho %d--------------", carpincho->pid);
   log_info(logger,"Tiempo de espera: %f", carpincho->tiempo.tiempo_de_espera);
   log_info(logger,"Tiempo ejecutado: %f", carpincho->tiempo.tiempo_ejecutado);
   log_info(logger,"Estimacion: %f", carpincho->tiempo.estimacion);
   //log_info(logger,"Time stamp inicio: %s", carpincho->tiempo.time_stamp_inicio);
   //log_info(logger,"Time stamp fin: %s\n", carpincho->tiempo.time_stamp_fin);
}

void listar_por_sjf(t_pcb *carpincho)
{
   if (!list_is_empty(lista_ordenada_por_algoritmo))
   {
      t_pcb *comparado;
      int i = 0;
      bool ok = false;
      while (i < list_size(lista_ordenada_por_algoritmo) && !ok)
      {
         comparado = (t_pcb *)list_get(lista_ordenada_por_algoritmo, i);
         //printf("SJF:\n");
         //mostrar_tiempos(comparado);
         mostrar_tiempos_log(comparado);
/*          double diferencia =  carpincho->tiempo.estimacion - comparado->tiempo.estimacion;
         if(diferencia < 0 && diferencia > -0.3){
            list_add_in_index(lista_ordenada_por_algoritmo, i+1, (void *)carpincho);
            ok = true;
         }else{ */
         if (carpincho->tiempo.estimacion < comparado->tiempo.estimacion)
         {
            list_add_in_index(lista_ordenada_por_algoritmo, i, (void *)carpincho);
            ok = true;
         }
         if (carpincho->tiempo.estimacion == comparado->tiempo.estimacion)
         {
            list_add_in_index(lista_ordenada_por_algoritmo, i + 1, (void *)carpincho);
            ok = true;
         }
         //}
         i++;
      }
      if (!ok)
      {
         list_add(lista_ordenada_por_algoritmo, (void *)carpincho);
      }
   }
   else
   {
      list_add(lista_ordenada_por_algoritmo, (void *)carpincho);
   }
}

void estimador(t_pcb *carpincho)
{
   //mostrar_tiempos(carpincho);
   mostrar_tiempos_log(carpincho);
   double aux = (carpincho->tiempo.estimacion * configuracion.ALPHA);
   carpincho->tiempo.estimacion = aux + (carpincho->tiempo.tiempo_ejecutado * (1 - configuracion.ALPHA));
   //mostrar_tiempos(carpincho);
   mostrar_tiempos_log(carpincho);
}

void estimador_HRRN(t_pcb* carpincho)
{
   //printf("entro a estimador hrrn\n");
   //mostrar_tiempos(carpincho);
   mostrar_tiempos_log(carpincho);
   
   if (carpincho->tiempo.estimacion != 0.000000)
   {
      carpincho->tiempo.estimacion = (carpincho->tiempo.tiempo_de_espera + carpincho->tiempo.estimacion) / carpincho->tiempo.estimacion;
   }
   //mostrar_tiempos(carpincho);
   mostrar_tiempos_log(carpincho);
   //log_info(logger, "Carpincho %d - Estimación actual: %f", carpincho->pid, carpincho->tiempo.estimacion);
  
}
double obtener_tiempo(t_pcb* carpincho)
{ 

   return (carpincho->tiempo_fin.tv_sec -carpincho->tiempo_inicio.tv_sec) +((double)((carpincho->tiempo_fin.tv_usec- carpincho->tiempo_inicio.tv_usec))*0.000001);
   // planteo asi esta funcion para poder usarla con el teimpo de espera tambien
 /*   int aux1, aux2;
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
           // total += (dif * 3600); //horas
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
   liberar_char(valores_inicio);
   liberar_char(valores_fin);
   return total; */
}

void liberar_char(char** lista){
int contador = 0;
while(lista[contador] != NULL){
        free(lista[contador]);
        contador++;
}
free(lista);
}

bool comparador_SFJ(void* arg1,void* arg2)
{t_pcb* carpincho1,* carpincho2;
   carpincho1 = (t_pcb*) arg1;
   carpincho2 = (t_pcb*) arg2;

   if (carpincho1->tiempo.estimacion < carpincho2->tiempo.estimacion)
   {
      return true;
   }
   return false;
}
void* comparador_HRRN(void *arg1,void* arg2)
{
   t_pcb* carpincho1,* carpincho2;
   carpincho1 = (t_pcb*) arg1;
   carpincho2 = (t_pcb*) arg2;
   if (carpincho1->tiempo.estimacion > carpincho2->tiempo.estimacion)
   {
      return (void*)carpincho1;
   } else
   {
     return (void*)carpincho2;
   } 
}
void bloquear_por_mediano_plazo(t_pcb *carpincho)
{
   //AVISAR A MEMORIA QUE EL CARPINCHO ESTA SUSPENDIDO (SWAMP)
   //printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||ENTRO A MEDIANO PLAZO CARPINCHO %d\n", carpincho->pid);
   //enviar_int(carpincho->fd_memoria, SUSPENCION);// ARREGLAR
   carpincho->estado = 'S';
   sem_wait(&mutex_cola_bloqueado_suspendido);
   queue_push(suspendido_bloqueado, carpincho);
   sem_post(&mutex_cola_bloqueado_suspendido);
   sem_post(&cola_suspendido_bloquedo_con_elementos);
   sem_post(&controlador_multiprogramacion);
}

void planificador_mediano_plazo()
{
   t_pcb *carpincho;
   while(!terminar)
   {
      sem_wait(&cola_suspendido_bloquedo_con_elementos);
      if(!terminar){
      sem_wait(&mutex_cola_bloqueado_suspendido);
      carpincho = (t_pcb *)queue_pop(suspendido_bloqueado);
      sem_post(&mutex_cola_bloqueado_suspendido);

      log_info(logger, "Se saca el carpincho %d de la cola bloqueado-suspendido", carpincho->pid);
      //printf("Se saca el carpincho %d a la cola bloqueado-suspendido", carpincho->pid);

      sem_wait(&carpincho->semaforo_fin_evento);
      //printf("--------------carpincho %d termino evento que lo suspendia\n", carpincho->pid);

      sem_wait(&mutex_cola_listo_suspendido);
      queue_push(suspendido_listo, carpincho);
      sem_post(&mutex_cola_listo_suspendido);
      sem_post(&cola_new_con_elementos);
      
      log_info(logger, "Se agrega el carpincho %d a la cola suspendido-listo porque recibió evento", carpincho->pid);
      //printf("Se agrega el carpincho %d a la cola suspendido-listo porque recibió evento\n", carpincho->pid);
   }
   }//printf("termino mediano plazo\n");
}

void iniciar_planificador_largo_plazo()
{
   t_pcb *carpincho = NULL;
   while(!terminar)
   {
      sem_wait(&controlador_multiprogramacion);
      if(!terminar)
      sem_wait(&cola_new_con_elementos);
      if(!terminar){
      log_info(logger, "Llegó un carpincho al planificador de largo plazo");
      if (!queue_is_empty(suspendido_listo))
      { //si el mediano plazo suspendio algo tiene prioridad es
       sem_wait(&mutex_cola_listo_suspendido);
         carpincho = (t_pcb*) queue_pop(suspendido_listo);
         sem_post(&mutex_cola_listo_suspendido);
         log_info(logger, "---------------Carpincho %d - Se saca de suspencion", carpincho->pid);
         //printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Carpincho %d - Se saca de suspencion\n", carpincho->pid);
        // enviar_mensaje_y_cod_op("sali de suspension", carpincho->fd_memoria, VUELTA_A_READY);// traer de swamp las paginasssssss
      }else{
         sem_wait(&mutex_cola_new);
         carpincho = (t_pcb*) queue_pop(cola_new);
         sem_post(&mutex_cola_new);
         inicializar_proceso_carpincho(carpincho);
         log_info(logger, "No hay suspendidos. Se iniciará carpincho %d", carpincho->pid);
      }
      //printf(" se va  a corotoplazo plp\n");
      iniciar_planificador_corto_plazo(carpincho);
   }
   }//printf("termino corto plazo\n");
}

void inicializar_proceso_carpincho(t_pcb *carpincho)
{
   carpincho->tiempo.estimacion = configuracion.ESTIMACION_INICIAL;
   carpincho->tiempo.tiempo_ejecutado = 0.000000;
   carpincho->tiempo.tiempo_de_espera = 0.000000;
  // carpincho->tiempo.time_stamp_inicio = NULL;
  // carpincho->tiempo.time_stamp_fin = NULL;
   carpincho->io_solicitada = NULL;
   carpincho->semaforo_a_modificar = NULL;
   carpincho->estado = 'N';
   sem_init(&carpincho->semaforo_evento, 0, 0);
   sem_init(&carpincho->semaforo_fin_evento, 0, 0);
}
/* void iniciar_gestor_finalizados()
{
   while(!terminar)
   {
      sem_wait(&cola_finalizados_con_elementos);
      sem_wait(&mutex_cola_finalizados);
      t_pcb *carpincho = (t_pcb*)queue_pop(cola_finalizados);
      sem_post(&mutex_cola_finalizados);
      printf("GESTOR DE FINALIZADOS: MATANDO carpincho %d\n", carpincho->pid);
      eliminar_carpincho((void *)carpincho);
      sem_post(&controlador_multiprogramacion);

   }printf("termino gestor finalizados\n");
} */
void eliminar_carpincho(void *arg)
{ // revisar que este este borrando lo necesario y no haya free's de mas
   t_pcb *carpincho = (t_pcb *)arg;
   sem_destroy(&carpincho->semaforo_evento);
   sem_destroy(&carpincho->semaforo_fin_evento);
   free(carpincho->io_solicitada);
      log_info(logger, "Carpincho %d - Eliminado", carpincho->pid);
  // printf("ELIMINAR CARPINCHO: CERRANDO CONEXION %d de carpincho %d\n", carpincho->fd_cliente, carpincho->pid);
   free(carpincho);
}

/* void ejecutando_a_bloqueado(t_pcb *carpincho, t_queue *cola, sem_t *mutex)
{
   carpincho->estado = 'B';
   carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);

   sem_wait(mutex);
   queue_push(cola, (void *)carpincho);
   sem_post(mutex);
} */

void bloqueado_a_listo(t_list *cola, sem_t *mutex)
{
   sem_wait(mutex);
   t_pcb *carpincho = (t_pcb*)list_remove(cola,0);
   //printf("bloqueado a listo carpincho %d\n", carpincho->pid);
   log_info(logger, "Bloqueado a listo Carpincho %d", carpincho->pid);
   sem_post(mutex);
   if(carpincho->estado == 'S'){
      sem_post(&carpincho->semaforo_fin_evento);
   }else
   {
      iniciar_planificador_corto_plazo(carpincho);
   }
   carpinchos_bloqueados --;
   /* sem_wait(&mutex_cola_ready);
   queue_push(cola_ready, (void *)carpincho);
   sem_post(&lista_ejecutando);
   log_info(logger, "Carpincho %d - Se saca de bloqueado", carpincho->pid);
   printf("BLOQUEADO_A_LISTO: desbloqueado CARPINCHO %d\n", carpincho->pid);
   sem_post(&cola_ready_con_elementos); */
   
   
}
