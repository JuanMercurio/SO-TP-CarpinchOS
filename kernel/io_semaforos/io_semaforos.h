#ifndef _IOSEMAFOROS_
#define _IOSEMAFOROS_
#include "main.h"

sem_kernel *buscar_semaforo(char *nombre);
//void sem_kernel_wait(char *nombre, t_pcb *carpincho);
bool sem_kernel_wait2(t_pcb *carpincho);
void sem_kernel_post(char *nombre);
void sem_kernel_init(char* nombre, int value);
void sem_kernel_destroy(char* nombre);
void bloquear_por_semaforo(t_pcb *carpincho);
bool verificar_bloqueo_por_semaforo(t_pcb *carpincho);
void sem_detroyer(void* semaforo);

io_kernel *buscar_io(char *nombre);
void init_dispositivos_io();
//void call_io(char *nombre, t_pcb *carpincho);
//void realizar_io(t_pcb *carpincho, io_kernel *io);
void bloquear_por_io(t_pcb *carpincho);
void inicial_hilos_gestores_de_io();
void gestor_cola_io(void *datos);
void bloquear_por_io(t_pcb *carpincho);

#endif
