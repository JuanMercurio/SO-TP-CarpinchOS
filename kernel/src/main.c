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

      case NEW_INSTANCE: ;
         t_pcb *carpincho = recibir_pcb(cliente); // aca no recibe la pcb en si , recibe un paquete con datos que habra que guardar en un t_pcb luego de desserializar lo que viene
         carpincho->fd_cliente = cliente;
         if (registrar_carpincho(carpincho) != -1)// comprobaciion por la memoria????
         {
            incializar_proceso_carpincho(carpincho);
   
            sem_wait(&mutex_cola_new);
            queue_push(cola_new, (void*) carpincho); // pensando que el proceso queda trabado en mate init hasta que sea planificado
            sem_post(&mutex_cola_new);
           // enviar_mensaje("OK", cliente);
         }// aca debe enviar el ok o debe planificarlo y al llegar a exec recien destrabar el carpincho


         else
            printf("no es posible crear estructuras para el carpincho\n");
         break;

      case IO:
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
void inciar_cpu(){
   pthread_t *cpu[configuracion.GRADO_MULTIPROGRAMACION];
   for(int i = 0 ; i < configuracion.GRADO_MULTIPROCESAMIENTO ; i++){
      pthread_create(cpu[i],NULL, (void*) procesador ,NULL);
   }

}
void procesador(){// resolver la cuestion de administacion de los semaforos de los procesos
   int operacion;
   while(1){
      sem__wait(&lista_ordenada_por_algoritmo);
      t_pcb *carpincho = (t_pcb*) list_take(lista_ordenada_por_algoritmo, 0);
      sem_pot(&lista_ordenada_por_algoritmo);
      carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");// al comenzar a ejecutar corta el eltiempo de espera
      carpincho->tiempo.tiempo_de_espera = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
      //aca el procesador lo ejecuta por lo tanto hay qu etomar el tiempo d einicio para depues tener lamdiferencia conel timep de salida
      strcpy(carpincho->tiempo.time_stamp_inicio,carpincho->tiempo.time_stamp_fin); // el momento que comeinza a ejecutar es el mismo que cuando termino la espera
      /*
      aca activaria al proeso peor no see si con semforo o mensaje
      */
      operacion = recivir_operacion(carpincho->fd_cliente); // con esto espera el mensaje que le llega del carpincho para ver como bloquaarlo;
      carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   }   
}
void iniciar_planificador_corto_plazo()
{ 
   while(1){
      sem_wait(&controlador_multiprogramacion);// este semaforo contador controla la cantidad de procesos que estaran ordenaod en ready el post lo hara el encargado de finalizar los procesos
      sem_wait(&cola_ready_con_elementos);
      t_pcb *carpincho = queue_pop(cola_ready);
      sem_post(&cola_ready_con_elementos);
      if(configuracion.ALGORITMO_PLANIFICACION == "SFJ"){
         estimador(carpincho);
        list_add_sorted(lista_ordenada_por_algoritmo, (void*)carpincho, (void*) comparador_SFJ); //falta, una vez enlistado se le cambia el estado
      }
      else
      {
         estimador_HRRN(carpincho);
         list_add_sorted(lista_ordenada_por_algoritmo, (void*)carpincho, (void*) comparador_HRRN);
      }
   }
}
void estimador(t_pcb *carpincho){// de donde se saca el valor de la rafaga de cpu
   if(carpincho->tiempo.time_stamp_inicio !=NULL){// si es null ES UN PROCESO NUEVO por lo tanto solo hace la etimacion
      carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
   }   
   carpincho->tiempo.estimacion = carpincho->tiempo.estimacion * configuracion.ALPHA + carpincho->tiempo.tiempo_ejecutado * (1 - configuracion.ALPHA);
}

void estimador_HRRN(t_pcb* carpincho){ // esto la complica bastante
   estimador(carpincho);
   carpincho->tiempo.estimacion = (carpincho->tiempo.tiempo_de_espera + carpincho->tiempo.estimacion) / carpincho->tiempo.estimacion;
}
double obtener_tiempo(char* inicio, char* fin){// planteo asi esta funcion para poder usarla con el teimpo de espera tambien
   int aux1, aux2;
	double dif;
	double total = 0.0;
   char ** valores_inicio = string_split(inicio, ":");
	char** valores_fin = string_split(fin, ":");

   for(int i = 0 ; i < 4 ;i ++){

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

bool comparador_SFJ(t_pcb* carpincho1, t_pcb* carpincho2){
   if(carpincho1->tiempo.estimacion < carpincho2->tiempo.estimacion){
      return true;
   }
   return false;
}
bool comparador_HRRN(t_pcb* carpincho1, t_pcb* carpincho2){
     if(carpincho1->tiempo.estimacion > carpincho2->tiempo.estimacion){
      return true;
   }
   return false;
}

void crear_estructuras(t_pcb *carpincho)
{
}
void iniciar_planificador_mediano_plazo()
{

}
void iniciar_planificador_largo_plazo()
{
   while (1)
   {
      sem_wait(&cola_new_con_elementos);
      sem_wait(&mutex_cola_new);
      t_pcb *carpincho = queue_pop(cola_new);
      sem_post(&mutex_cola_new);
      crear_estructuras(carpincho); // que estructura hay que crear?? si en la pcb se guardan las estimaciones semaforos??
      sem_wait(&mutex_cola_ready);
      queue_push(cola_ready, carpincho);
      carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS");
      sem_post(&mutex_cola_ready);
   }
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

void sem_kernel_wait(char *nombre)
{
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   //agregar mutex{
   sem->val =-1;
  
   //loguear
   if (sem->val < 1)
   {
      //bloquear proceso
   }
   free(sem);
}

void sem_kernel_post(char *nombre) 
{
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   //agregar mutex
   sem->val = +1;

   //loguear
   if (sem->val == 0) //¿tiene que ser 0 o 1?
   { 
      //desloquear proceso
   }
   free(sem);
}

void sem_kernel_init(char* nombre, int value){
   sem_kernel *nuevo_sem = malloc(sizeof(sem_kernel));
   nuevo_sem->id=nombre;
   nuevo_sem->max_val=value;
   nuevo_sem->val=value;
   nuevo_sem->bloqueados=queue_create();
   list_add(lista_sem_kernel, nuevo_sem);
   free(nuevo_sem);
}

void sem_kernel_destroy(char* nombre){
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   
   bool nombre_semaforo(void* elemento){
      return (((sem_kernel*)elemento)->id == nombre);
   }

   //revisar si está bien planteado
   list_remove_by_condition(lista_sem_kernel, nombre_semaforo);
}

void init_dispositivos_io(){};
void call_io(char *nombre){};