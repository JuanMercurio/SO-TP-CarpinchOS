#ifndef CONFIG_PAG_H
#define CONFIG_PAG_H

#include "../configuracion/config.h"
#include "algoritmos.h"
#include "estructuras.h"

extern int swap;

void set_algoritmos();
void set_alg_principal();
void set_alg_tlb();
int set_campo_algoritmo();
void asignacion_swap();
void swap_conexion_init(int server);


#endif
