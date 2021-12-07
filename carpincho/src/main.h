#ifndef _CARPINCHO_
#define _CARPINCHO_

void carpincho_comportamiento(void* arg);
void carpincho_comportamiento2(void* arg);
void carpincho_comportamiento3(void *arg);
void carpincho_comportamiento_memoria(void* arg);
void* carpincho1_func(void* config);
void* carpincho2_func(void* config);
void* carpincho3_func(void* config);
void* carpincho4_func(void* config);
void memoria_carpincho();

//-----------------------------------------------------
void imprimir_carpincho_n_hace_algo(int numero_de_carpincho);
void exec_carpincho_1(char *config);

void exec_carpincho_2(char *config);

void exec_carpincho_3(char *config);

void free_all();

void init_sems();
#endif