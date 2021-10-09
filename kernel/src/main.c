#include "main.h"

int main(int argc, char *argv[])
{

   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);

   obtener_config();
   administrar_clientes(configuracion.IP, configuracion.PUERTO, (void *)receptor);
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

      case NUEVO_CARPINCHO: ;
         t_pcb *carpincho = recibir_pcb(cliente);
         if (registrar_carpincho(carpincho) != -1)
         {
            enviar_ok(cliente);
         }
         else
            printf("no es posible crear estructuras para el carpincho\n");
         break;

      case IO:
         break;
      }
   }
}

int recibir_operacion(int cliente){}
int recibir_pcb(int cliente){}

void inicializar_planificacion()
{
   iniciar_colas();
   iniciar_planificador_corto_plazo();
   iniciar_planificador_mediano_plazo();
   iniciar_planificador_largo_plazo();
}
void iniciar_colas()
{
   cola_new = queue_create();
   cola_ready = queue_create();
   suspendido_bloqueado = queue_create();
   suspendido_listo = queue_create();
}

void iniciar_planificador_corto_plazo()
{
   while (1)
   {
      sem_wait(&cola_new_con_elementos);
      sem_wait(&mutex_cola_new);
      t_pcb *carpincho = queue_pop(cola_new);
      sem_post(&mutex_cola_new);
      crear_estructuras(carpincho);
      sem_wait(&mutex_cola_ready);
      queue_push(cola_ready, carpincho);
      sem_post(&mutex_cola_ready);
   }
}
void crear_estructuras(t_pcb *carpincho)
{
}
void iniciar_planificador_mediano_plazo()
{
}
void iniciar_planificador_largo_plazo()
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

sem_kernel *buscar_semaforo(char *nombre, t_list *sems)
{
   sem_kernel *sem_buscado = malloc(sizeof(sem_kernel));

   bool nombre_semaforo(void* elemento){
      return true;//((sem_kernel*)elemento)->id == nombre;
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