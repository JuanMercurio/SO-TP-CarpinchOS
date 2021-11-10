#ifndef _IOSEMAFOROS_
#define _IOSEMAFOROS_

#include "../main.h"
#include "../planificacion/planificacion.h"
#include "../configuracion/config.h"

sem_kernel *buscar_semaforo(char *nombre);
io_kernel *buscar_io(char *nombre);

void bloquear_por_io(t_pcb *carpincho);
void call_io(char *nombre, t_pcb *carpincho);
void gestor_cola_io(void *datos);
void init_dispositivos_io();
void realizar_io(t_pcb *carpincho, io_kernel *io);
void iniciar_hilos_gestores_de_io();

int obtener_valor_semaforo(char* semaforo_a_modificar);

void sem_kernel_destroy(char* nombre);
void sem_kernel_init(char* nombre, int value);
void sem_kernel_post(char *nombre, t_pcb *carpincho);
void sem_kernel_wait(char *nombre, t_pcb *carpincho);
void sem_kernel_wait2(t_pcb *carpincho);

#endif
