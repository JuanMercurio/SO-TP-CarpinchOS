#ifndef _PLANIFICACION_
#define _PLANIFICACION_

#include "../main.h"
#include "../configuracion/config.h"

t_pcb *ultimo_bloqueado;
extern int carpinchos_bloqueados;

/* 
    @NAME: inciar_cpu
    @DESC: incia instancias de cpu segun el grado de
           multiprogramacion creando hilos con la funcion procesador.
 */
void inciar_cpu();

/* 
    @NAME: procesador
    @DESC: toma un puntero de la lista ordenada por algoritmo
           le coloca el estado E y lo habilita a ejecutar. Toma
           timestamps para controlar estimaciones.
 */
void procesador();

void planificador_mediano_plazo(t_pcb *carpincho);

void eliminar_carpincho(t_pcb *carpincho);// revisar que este este borrando lo necesario y no haya free's de mas

/* 
    @NAME: inciar_planifiacdor_corto_plazo
    @DESC: quita un puntero de la cola de listos y lo planifica
           enlistandolo ordenadamente segun el algoritmo de planificacion.
 */
void iniciar_planificador_corto_plazo();
void iniciar_planificador_mediano_plazo();

/* 
    @NAME: inciar_planificador_largo_plazo
    @DESC: toma elementos de la cola new y los pasa a la cola de listos
    luego de generar las estructuras necesarias.
 */
void iniciar_planificador_largo_plazo();

/*
    @NAME: iniciar_gestor_finalizados
    @DESC: funcion encargada de liberar la memoria d elos procesos finalizados
 */
void iniciar_gestor_finalizados();

bool comparador_HRRN(t_pcb* carpincho1, t_pcb* carpincho2);
bool comparador_SFJ(t_pcb* carpincho1, t_pcb* carpincho2);

/* 
    @NAME: obtener_tiempo
    @DESC: dadas dos cadenas de time stamps retorna la diferencia en segundos.
 */
double obtener_tiempo(char* inicio, char* fin);

void estimador_HRRN(t_pcb* carpincho);

void estimador(t_pcb *carpincho);

void ejecutando_a_bloqueado(t_pcb*,t_queue *,sem_t*);
void bloqueado_a_listo(t_pcb*,t_queue *,sem_t*);

bool verificar_suspension();
 
#endif