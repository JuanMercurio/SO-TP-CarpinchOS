#ifndef _DEADLOCK_
#define _DEADLOCK_

typedef struct{
   int pid;
   char* retenido;
   char* esperando;
}t_deadlock;


void deteccion_deadlock();
#endif