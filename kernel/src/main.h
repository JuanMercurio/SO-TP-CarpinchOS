#ifndef _MAIN_KERNEL_
#define _MAIN_KERNEL_

#include "tests/tests.h"
#include "configuracion/config.h"
#include <pthread.h>
#include <utils/utils.h>
#include "conexiones/conexiones.h"
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/temporal.h>
#include <mensajes/mensajes.h>
#include <stdio.h>
#include <sys/time.h>

int servidor;
extern bool terminar;
extern int carpinchos_bloqueados;
sem_t mate_close;
// type struct
struct timeval *tiempito;
struct tm *aux;
typedef struct tiempo_t{
  //  double estimacion_anterior;
    double tiempo_ejecutado;
    double estimacion; // en  hrrn el tiepo de servicio es la estimacion para la proximaejecucion
    double tiempo_de_espera;

}tiempo_t;

typedef struct io_kernel{
    char* id;
    int retardo;
    t_queue* bloqueados;
    sem_t mutex_io;
    sem_t cola_con_elementos;
}io_kernel;

typedef struct deadlock_kernel{
   int pid;
   char* retenido;
   char* esperando;
}deadlock_kernel;

typedef struct sem_kernel{
    char* id;
    int val;
    int max_val;
    t_list* bloqueados;
    sem_t mutex;
    sem_t mutex_cola;
    int tomado_por;
}sem_kernel;

typedef struct t_pcb{
    int pid;
    int fd_cliente;
    int fd_memoria;
    tiempo_t tiempo;
    char estado;
    int proxima_instruccion;
    char* io_solicitada;
    io_kernel* io;
    char* semaforo_a_modificar;
    sem_t semaforo_evento;
    sem_t semaforo_fin_evento;
    sem_kernel* bloqueado_en;
    struct timeval tiempo_inicio;
    struct timeval tiempo_fin;
}t_pcb;

typedef enum{
    NEW_INSTANCE,
    IO,
    INIT_SEMAFORO,
    SEM_WAIT,
    SEM_POST,
    SEM_DESTROY,
    MEMALLOC,
    MEMFREE,
    MEMREAD,
    MEMWRITE,
    MATE_CLOSE,
    NEW_INSTANCE_KERNEL,
    SUSPENCION,
    VUELTA_A_READY
}cod_op;





//   colas
t_queue *cola_new;
t_queue *cola_ready;
t_queue *suspendido_bloqueado;
t_queue *suspendido_listo;
t_queue *cola_finalizados;
// listas
//t_list *lista_new;
t_list *lista_ejecutando;
t_list *lista_sem_kernel;
t_list *lista_io_kernel;
t_list *lista_ordenada_por_algoritmo;

// semaforos
sem_t cola_new_con_elementos;
sem_t cola_ready_con_elementos;
sem_t cola_suspendido_bloquedo_con_elementos;
sem_t cola_suspendido_listo_con_elementos;
sem_t lista_ejecutando_con_elementos;
sem_t cola_finalizados_con_elementos;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_cola_bloqueado_suspendido;
sem_t mutex_cola_listo_suspendido;
sem_t mutex_lista_ejecutando;

sem_t mutex_cola_finalizados;
sem_t mutex_lista_oredenada_por_algoritmo;
sem_t controlador_multiprogramacion;

sem_t mutex_lista_sem_kernel;
sem_t mutex_lista_io_kernel;
sem_t semaforo_sicro_fin_deadlock;


 pthread_attr_t detached2;
void administrar_clientes_kernel(char* IP, char* PUERTO, void (*funcion)(void*));
/*
    @NAME: terminar_programa
    @DESC: Se encarga de liberar todas las estructuras y de hacer lo necesario para
           que el programa termine sin memory leaks y sin errores
 */
void terminar_programa();

void iniciar_colas();

void incializar_planificacion();
   
//void crear_estructuras(t_pcb *carpincho);

void inicializar_listas_sem_io();

void receptor(void*);

void inicializar_semaforos();

void inicializar_listas_sem_io();

void destruir_colas_y_listas();

void destruir_semaforos();

void inicializar_planificacion();

void program_killer();
void matar();


//-------------------------------------------------sancocho


//_____________________________________________________________deadlock

//----------------------------------------------------------planificacion


#endif