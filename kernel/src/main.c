#include "main.h"
#include "io_semaforos/io_semaforos.h"
#include "planificacion/planificacion.h"
#include "deadlock/deadlock.h"
#include "signal.h"
bool terminar = false;


int id_procesos = 0;
int carpinchos_bloqueados = 0;

void signal_init(int sig, void(*handler)){ 
    struct sigaction sa;
    //sa.sa_mask ;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    sigaction(sig, &sa, NULL);
}
void signal_usr1(int sig, void(*handler)){ 
    struct sigaction sa;
    //sa.sa_mask = 0;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    sigaction(sig, &sa, NULL);

}
int main(int argc, char *argv[])
{
   //solo corre si corremos el binario asi: binario test
   //tests(argc, argv[1]);
   signal_init(SIGINT, (void*)program_killer);
   signal_usr1(SIGUSR1, (void*) matar);
   iniciar_logger();
   printf("Iniciando kernel\n");
   obtener_config();
   //printf("inicio2\n");
   inicializar_semaforos();
   inicializar_listas_sem_io();
   init_dispositivos_io();
   //printf("inicio dispositivos io\n");
   inicializar_planificacion();
   printf("Kernel iniciado\n");
   administrar_clientes_kernel(configuracion.IP, configuracion.PUERTO, (void *)receptor); 
   printf("salio de atender clientes\n");
  /*  terminar = true;
   terminar_programa(); */

   sem_init(&semaforo_sicro_fin_deadlock, 0, 0);
   sem_wait(&semaforo_sicro_fin_deadlock);
   printf("paso wait\n");
   sem_destroy(&semaforo_sicro_fin_deadlock);
   log_destroy(logger);
   printf("destruye logger\n");
   return 0;
}

void terminar_programa()
{
   printf("Terminando kernel\n");
   while(!list_is_empty(lista_io_kernel)){
      io_kernel * destroy = list_remove(lista_io_kernel,0);
      sem_post(&destroy->cola_con_elementos);
      //printf("post a io %s\n", destroy->id);
      io_destroyer((void*)destroy);
     
     
      //printf("destruyo io\n");
   }
   list_destroy(lista_io_kernel);
   sem_post(&cola_suspendido_bloquedo_con_elementos);
   sem_post(&controlador_multiprogramacion);
   sem_post(&cola_new_con_elementos);
   for(int i = 0; i < configuracion.GRADO_MULTIPROGRAMACION; i ++){// parche
   sem_post(&lista_ejecutando_con_elementos);
}

   //printf("Entro a destrucción config\n");
   config_destroy(config);
   //printf("Entro a destrucción logger\n");
   
   //printf("Entro a destrucción semaforos\n");
   destruir_semaforos();
   //printf("Entro a destrucción colas y listas\n");
   destruir_colas_y_listas();
   //printf("destuyendo detacheds\n");
   pthread_attr_destroy(&detached2);
   //printf("cerrando servidor...\n");
   close(servidor);
   printf("Kernel finalizado\n");
}
void matar(){
   printf("mato el hilo acpertador??\n");
}

void destruir_colas_y_listas()
{
   if (!list_is_empty(lista_sem_kernel))
   {
      list_destroy_and_destroy_elements(lista_sem_kernel, (void *)sem_destroyer);
      //printf("destruyo lista sem kernel con elementos\n");
   }
   else
   {
      list_destroy(lista_sem_kernel);
            //printf("destruyo lista sem kernel sin elementos\n");

   }
   
   if (!queue_is_empty(cola_new)) // cambiado
   {
      queue_destroy_and_destroy_elements(cola_new, (void *)eliminar_carpincho);
            //printf("destruyo cola new con elementos\n");
   }
   else
   {
      queue_destroy(cola_new);
                  //printf("destruyo cola new sin elementos\n");
   }
   if (!queue_is_empty(suspendido_bloqueado))
   {
      queue_destroy_and_destroy_elements(suspendido_bloqueado, (void *)eliminar_carpincho);
            //printf("destruyo suspendo bloquedo con elementos\n");

   }
   else
   {
      queue_destroy(suspendido_bloqueado);
      //printf("destruyo suspendo bloquedo sin elementos\n");
   }
   if (!queue_is_empty(suspendido_listo))
   {
      queue_destroy_and_destroy_elements(suspendido_listo, eliminar_carpincho);
          //  printf("destruyo suspendo listo con elementos\n");
   }
   else
   {
      queue_destroy(suspendido_listo);
                 // printf("destruyo suspendo listo sin elementos\n");
   }
   if (!list_is_empty(lista_ordenada_por_algoritmo))
   {
      list_destroy_and_destroy_elements(lista_ordenada_por_algoritmo, (void *)eliminar_carpincho);
                    //    printf("destruyo lista ordenaod por algoritmo con elementos\n");

   }
   else
   {
      list_destroy(lista_ordenada_por_algoritmo);
                     //         printf("destruyo lista ordenaod por algoritmo sin elementos\n");

   }
   log_info(logger, "Colas, listas y sus respectivos elementos destruidos");
   printf("Destruidas las listas y colas\n");
}

void destruir_semaforos()
{
    //holoosoksoajaojoa
   sem_destroy(&cola_new_con_elementos);
  // sem_destroy(&cola_ready_con_elementos);
   sem_destroy(&cola_suspendido_bloquedo_con_elementos);
   sem_destroy(&cola_suspendido_listo_con_elementos);
   //sem_destroy(&cola_finalizados_con_elementos);
   sem_destroy(&mutex_cola_new);
 //  sem_destroy(&mutex_cola_ready);
   sem_destroy(&mutex_cola_bloqueado_suspendido);
   sem_destroy(&mutex_cola_listo_suspendido);
  // sem_destroy(&mutex_lista);
  // sem_destroy(&mutex_cola_finalizados);
   sem_destroy(&mutex_lista_oredenada_por_algoritmo);
   sem_destroy(&controlador_multiprogramacion);
   printf("se rompe aca\n");
   sem_destroy(&lista_ejecutando_con_elementos);
   //log_info(logger, "Semáforos destruidos");
}

void receptor(void *arg)
{
   printf("RECEPTOR DE CLIENTE %d\n", *(int *)arg);
   log_info(logger,"Recibí el cliente %d",*(int *)arg);
   int cliente = *(int *)arg;
   free(arg);
   int aux_int = 0;
   int verificador = 0;
   bool conectado = true;
   //printf("por ahcer handshake\n");
   handshake(cliente, "KERNEL");
   t_pcb *carpincho = NULL;
   t_paquete_semaforo *semaforo;
   char *recibido;
   int operacion = 0;

   while (conectado)
   {
      operacion = recibir_operacion(cliente);
      switch (operacion)
      {

      case NEW_INSTANCE:
         log_info(logger, "---------------NEW INSTANCE. Comienza creación del carpincho");
         carpincho = malloc(sizeof(t_pcb)); // aca no recibe la pcb en si , recibe un paquete con datos que habra que guardar en un t_pcb luego de desserializar lo que viene
         carpincho->fd_cliente = cliente;
         carpincho->pid = crearID(&id_procesos);
         carpincho->pid++;
         printf("CLIENTE %d CARPINCHO %d\n", cliente, carpincho->pid);
         carpincho->estado = 'N';
        // verificador = 66;
            carpincho->fd_memoria = crear_conexion(configuracion.IP_MEMORIA, configuracion.PUERTO_MEMORIA);
            enviar_cod_op_e_int(carpincho->fd_memoria, NEW_INSTANCE_KERNEL, carpincho->pid);
            
            recibido = recibir_mensaje(carpincho->fd_memoria); //handshake
            free(recibido);
            aux_int = recibir_int(carpincho->fd_memoria);
            printf("recibio de memoria un %d\n", aux_int);
            if (aux_int == 0)
            {  
         //printf("carpincho creado\n");
         enviar_int(cliente, carpincho->pid);
         sem_wait(&mutex_cola_new);
         queue_push(cola_new, carpincho);
         sem_post(&mutex_cola_new);
         log_info(logger, "NEW INSTANCE: Se agregó el carpincho ID: %d a la cola de new", carpincho->pid);
         sem_post(&cola_new_con_elementos);
            }else{
                  enviar_mensaje(cliente, "FAIL");
               }  
         break;

      case INIT_SEMAFORO:
         if (carpincho->pid < 0)
         {enviar_int(cliente,-1);
            break;
         } // SE PUEDE MODIFICAR PARA CONFIRMAR  MAL
         //printf("MAIN:recibi un init semaforo\n");
         semaforo = recibir_semaforo(cliente); // recibe el puntero
         log_info(logger, "Se recibió del carpincho %d un SEM INIT para el semáforo %s con valor %d\n ", carpincho->pid, semaforo->nombre_semaforo, semaforo->valor);
         int resultado = sem_kernel_init(semaforo->nombre_semaforo, semaforo->valor); // usa lo que necesit
         enviar_int(cliente, resultado);                                              // responde peticion con ok
         //free(semaforo->buffer->stream);  
         free(semaforo->nombre_semaforo);                                       // bora lo que alloco
         free(semaforo);

         break;
      case IO:
         if (carpincho->pid < 0)
         {  recibido = recibir_mensaje(cliente);
         free(recibido);
            enviar_int(cliente,-1);
            break;
         }
         recibido = recibir_mensaje(cliente);
         carpincho->proxima_instruccion = IO;
         carpincho->io = buscar_io(recibido);
         printf("RECEPTOR: io: recibido de bloquear por io a carpincho %d\n", carpincho->pid);         if (carpincho->io == NULL)
         {
            enviar_int(cliente, -1);
         }
         else
         {
            sem_post(&carpincho->semaforo_evento);
            enviar_int(carpincho->fd_cliente, 0);
            log_info(logger, "Se recibió del carpincho %d un CALL IO para %s", carpincho->pid, recibido);
         }
         free(recibido);
         break;

      case SEM_WAIT:
         if (carpincho->pid < 0)
         {  recibido = recibir_mensaje(cliente);
         free(recibido);
            enviar_int(cliente,-1);
            break;
         }
         recibido = recibir_mensaje(cliente);
          carpincho->proxima_instruccion = SEM_WAIT;
         int pos;
         carpincho->bloqueado_en = buscar_semaforo2(recibido, &pos);

         printf("RECEPTOR: --------------------------------------- recibido %s\n", recibido);
         free(recibido);
         if (carpincho->bloqueado_en == NULL)
         {
            //printf("RECEPTOR: el semaforo no se encuentra\n");
            log_info(logger, "Se recibió del carpincho %d un SEM WAIT para un semáforo que no existe", carpincho->pid);
            enviar_int(cliente, -1);
         }
         else
         {
            //printf("RECEPTOR: el semaforo encontradoe es %s\n", sem->id);
            if (sem_kernel_wait2(carpincho))
            {         printf("RECEPTOR: ---------------------------------------paso sem wait2 \n");

               sem_post(&carpincho->semaforo_evento);
               enviar_int(cliente, 0);
               log_info(logger, "Se recibió del carpincho %d un SEM WAIT para %s y se bloqueara", carpincho->pid, carpincho->bloqueado_en->id);
            }
            else
            {
               enviar_int(cliente, 1);
             //  log_info(logger, "Se recibió del carpincho %d un SEM WAIT para %s pero NO se bloqueara", carpincho->pid, carpincho->bloqueado_en->id);
            }
         }
         break;

      case SEM_POST:
         if (carpincho->pid < 0)
         {printf("MANDO -1 POSSSSSSSSSSSSSSSSSSST\n");
         recibido = recibir_mensaje(cliente);
         free(recibido);
            enviar_int(cliente,-1);
            break;
         }
         recibido = recibir_mensaje(cliente);
        // log_info(logger, "Se recibió del carpincho %d un SEM POST para %s", carpincho->pid, recibido);
         printf("Se recibió del carpincho %d un SEM POST para %s", carpincho->pid, recibido);
         //printf("MAIN:recibi un post semaforo para %s DEL CARPINCHO %d cliente %d\n", recibido, carpincho->pid, cliente);
         if (recibido == NULL)
            break;
         resultado = sem_kernel_post(recibido);
         //printf("el resultado del post en el main kernel es %d \n", resultado);
         enviar_int(cliente, resultado);
         //printf("mande resultado kernel post\n");
         free(recibido);
         break;

      case SEM_DESTROY:
         if (carpincho->pid < 0)
         {recibido = recibir_mensaje(cliente);
         free(recibido);
            enviar_int(cliente,-1);
            break;
         }
         recibido = recibir_mensaje(cliente);
         log_info(logger, "Se recibió del carpincho %d un SEM DESTROY para %s", carpincho->pid, recibido);
         //printf("sem del destroy %s\n", recibido);
         aux_int = sem_kernel_destroy(recibido);
         printf("RECEPTOR: SEM DESTROY auxint %d\n", aux_int);
         enviar_int(cliente, aux_int);
         free(recibido);
         break;

      case MEMALLOC:
         if (carpincho->pid < 0)
         {recibir_int(cliente);
         enviar_int(cliente, -1);
            break;
         }
         aux_int = recibir_int(cliente);
         log_info(logger, "Se recibió del carpincho %d un MEM ALLOC con el tamañi %d", carpincho->pid, aux_int);
         printf("Se recibió del carpincho %d un MEM ALLOC con el tamañi %d\n", carpincho->pid, aux_int);
         enviar_cod_op_e_int(carpincho->fd_memoria, MEMALLOC, aux_int);
         aux_int = recibir_int(carpincho->fd_memoria);
         printf("la direccion logica que me llego es-- %d\n\n", aux_int);
         enviar_int(cliente, aux_int);

         break;

      case MEMFREE:
         if (carpincho->pid < 0)
         {recibir_int(cliente);
         enviar_int(cliente, -5);
            break;
         }
         aux_int = recibir_int(cliente);
         log_info(logger, "Se recibió del carpincho %d un MEM FREE con el tamanio %d", carpincho->pid, aux_int);
         printf("Se recibió del carpincho %d un MEM FREE con el tamanio %d\n", carpincho->pid, aux_int);
         enviar_cod_op_e_int(carpincho->fd_memoria, MEMFREE, aux_int);
         aux_int = recibir_int(carpincho->fd_memoria);
         enviar_int(cliente, aux_int);
         break;

      case MEMREAD:
         if (carpincho->pid < 0)
         {recibir_int(cliente);
         recibir_int(cliente);
            enviar_int(cliente, -6);
            break;
         }
         aux_int = recibir_int(cliente);
         int size = recibir_int(cliente);
         log_info(logger, "Se recibió del carpincho %d un MEM READ desde la posición %d con tamañio %d", carpincho->pid, aux_int, size);
         printf("Se recibió del carpincho %d un MEM READ desde la posición %d con tamañio %d\n", carpincho->pid, aux_int, size);

         enviar_cod_op_e_int(carpincho->fd_memoria, MEMREAD, aux_int);
         enviar_int(carpincho->fd_memoria, size);

         aux_int = recibir_int(carpincho->fd_memoria);

         if (aux_int == -1)
         {
            enviar_int(carpincho->fd_memoria, -1);
            break;
         }

         int size_confimation = recibir_int(carpincho->fd_memoria);
         recibido = recibir_buffer(size_confimation, carpincho->fd_memoria);

         // enviar_mensaje(cliente, recibido);
         enviar_int(cliente, 0);
         enviar_int(carpincho->fd_cliente, size_confimation);
         enviar_buffer(cliente, recibido, size_confimation);

         printf("MEMREAD mensaje enviado %s\n ", recibido);
         free(recibido);
         break;

      case MEMWRITE:
         if (carpincho->pid < 0)
         {recibido = recibir_mensaje(cliente);
         free(recibido);
         recibir_int(cliente);
         enviar_int(cliente, -7);
            break;
         }
         int buffer_size = recibir_int(carpincho->fd_cliente);
         void* recibido = recibir_buffer(buffer_size, carpincho->fd_cliente);
         aux_int = recibir_int(cliente);
         printf("me llego del carpincho %s\n", recibido);

         enviar_int(carpincho->fd_memoria, MEMWRITE);
         enviar_int(carpincho->fd_memoria, buffer_size);
         enviar_buffer(carpincho->fd_memoria, recibido, buffer_size);
         enviar_int(carpincho->fd_memoria, aux_int);
         log_info(logger, "Se recibió del carpincho %d un MEM write desde la posición %d con el mensjaje %s", carpincho->pid, aux_int, recibido);
         printf("-----------------------------esperando respuesta memoria\n");
         aux_int = recibir_int(carpincho->fd_memoria);
         printf("aux int %d\n", aux_int);
         enviar_int(cliente, aux_int);
         free(recibido);
         break;

      case MATE_CLOSE:
         if (carpincho->pid < 0)
         {
            log_info("---------------MATE CLOSE - Receptor terminando conexion con %d. ELIMINADO POR DEADLOCK\n", cliente);
            printf("---------------MATE CLOSE - Receptor terminando conexion con %d. ELIMINADO POR DEADLOCK\n", cliente);
            conectado = false;
            eliminar_carpincho(carpincho);
         }else{
         log_info(logger, "MATE_CLOSE: Se recibió del carpincho %d un MATE CLOSE", carpincho->pid);
         enviar_int(carpincho->fd_memoria, MATE_CLOSE);
         carpincho->proxima_instruccion = MATE_CLOSE;
         sem_post(&carpincho->semaforo_evento);
         conectado = false;
         eliminar_carpincho(carpincho);
        // aux_int =0;
         printf("---------------MATE CLOSE - Receptor terminando conexion con %d. Carpincho fue asado\n", cliente);
         }
         break;

      default:
      if (carpincho->pid < 0){
         break;
      }else{
         printf("codigo erroneo DEL CLIENTE %d\n", cliente);
         log_info(logger,"Se recibió un código erróneo del cliente %d", cliente);
         conectado = false;
         break;
      }
      }
   }
   printf("MAIN KERNEL SALIO DEL WHILEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
   close(cliente);
   sem_post(&controlador_multiprogramacion);
   printf("RECEPTOR: saliendo------------------------------------------------------------\n");
   return;
}

void inicializar_planificacion()
{
   pthread_t hilos_planificadores;
   pthread_attr_setdetachstate(&detached2, PTHREAD_CREATE_DETACHED);

   iniciar_colas();
   log_info(logger, "Inicio colas de planificación");
   if (pthread_create(&hilos_planificadores, &detached2, (void *)planificador_mediano_plazo, NULL) != 0)
   {
      log_info(logger, "No se pudo crear el hilo Planificador Corto Plazo");
   }
   else
   {
      log_info(logger, "Hilo Planificador Corto Plazo creado");
   }

   if (pthread_create(&hilos_planificadores, &detached2, (void *)iniciar_planificador_largo_plazo, NULL) != 0)
   {
      log_info(logger, "No se pudo crear el hilo Planificador Largo Plazo");
   }
   else
   {
      log_info(logger, "Hilo Planificador Largo Plazo creado");
   }

   if (pthread_create(&hilos_planificadores, &detached2, (void*)deadlock, NULL) != 0)
   {
      log_info(logger, "No se pudo crear el hilo Detección Deadlock");
   }
   else
   {
      log_info(logger, "Hilo Detección Deadlock creado");
   }

   if (pthread_create(&hilos_planificadores, &detached2, (void *)iniciar_cpu, NULL) != 0)
   {
      log_info(logger, "No se pudo crear el hilo CPU");
   }
   else
   {
      log_info(logger, "Hilo CPU creado");
   }
}
void program_killer()
{
   terminar = true;
   printf("terminar = true\n");
   kill(getpid(), SIGUSR1);
   printf("envia SIGUSR1\n");
   terminar_programa();
   printf("TERMINO SERVIDOR\n");
}

void iniciar_colas()
{
   cola_new = queue_create(); // cambio
  // cola_ready = queue_create();
   suspendido_bloqueado = queue_create();
   suspendido_listo = queue_create();
 //  cola_finalizados = queue_create();
   lista_ordenada_por_algoritmo = list_create();
}
void inicializar_semaforos()
{

   sem_init(&cola_new_con_elementos, 0, 0);
  // sem_init(&cola_ready_con_elementos, 0, 0);
   sem_init(&cola_suspendido_bloquedo_con_elementos, 0, 0);
   sem_init(&cola_suspendido_listo_con_elementos, 0, 0);
  // sem_init(&cola_finalizados_con_elementos, 0, 0);

   sem_init(&mutex_cola_new, 0, 1);
  // sem_init(&mutex_cola_ready, 0, 1);
   sem_init(&mutex_cola_bloqueado_suspendido, 0, 1);
   sem_init(&mutex_cola_listo_suspendido, 0, 1);
  // sem_init(&mutex_lista_ejecutando, 0, 1);
  // sem_init(&mutex_cola_finalizados, 0, 1);
   sem_init(&mutex_lista_oredenada_por_algoritmo, 0, 1);
   sem_init(&mutex_lista_sem_kernel, 0, 1);
   sem_init(&mutex_lista_io_kernel, 0, 1);
   sem_init(&controlador_multiprogramacion, 0, configuracion.GRADO_MULTIPROGRAMACION);
   log_info(logger, "Semáforos inicializados");
}
void inicializar_listas_sem_io()
{
   lista_io_kernel = list_create();
   lista_sem_kernel = list_create();
   //printf("CREO LISTA IO Y KERNEL SEM\n");
}

void administrar_clientes_kernel(char* IP, char* PUERTO, void (*funcion)(void*)){

   servidor = iniciar_servidor(IP, PUERTO);

   pthread_attr_t detached;
   pthread_attr_init(&detached);
   pthread_attr_setdetachstate(&detached, PTHREAD_CREATE_DETACHED);

   /* Revisar Condicion para terminar este while */
   while(!terminar){
      pthread_t hilo;
      int *cliente = malloc(sizeof(int)); // posible memory leaking
      *cliente= aceptar_cliente(servidor);
      if(*cliente == -1){
         free(cliente);
         printf("salio por cliente -1 atender!!!1\n");
      }else{
      pthread_create(&hilo, &detached, (void*)funcion,(void*) cliente);
      }
   
}
printf("termino atender clientes\n");
   pthread_attr_destroy(&detached); 

}
