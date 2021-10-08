#include "main.h"

int main(int argc, char *argv[])
{

   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);

   obtener_config();
   administrar_clientes2(configuracion.IP, configuracion.PUERTO, (void *)receptor);
   inicializar_planificacion();

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

      case NUEVO_CARPINCHO:
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

void incializar_planificacion()
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

sem_kernel *buscar_semaforo(char *nombre, t_list *sems)
{
   sem_kernel *sem_buscado = malloc(sizeof(sem_kernel));

   //inner function para buscar el semaforo con el nombre recibido
   bool nombre_semaforo(void* elemento){
      return (((sem_kernel*)elemento)->id == nombre);
   }
   //uso la inner function porque list_find requiere un void* como condicion
   sem_buscado = list_find(sems, nombre);
   return sem_buscado;
}

void sem_wait(char *nombre)
{
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   sem->val =-1;
   //loguear
   if (sem->val < 1)
   {
      //bloquear proceso
   }
}

void sem_post(char *nombre)
{
   sem_kernel *sem = buscar_semaforo(nombre, lista_sem_kernel);
   sem->val = +1;
   //loguear
   if (sem->val == 0) //¿tiene que ser 0 o 1?
   { 
      //desloquear proceso
   }
}

void sem_init(char* nombre, int value){}
void sem_destroy(char* nombre){}
void init_dispositivos_io(){};