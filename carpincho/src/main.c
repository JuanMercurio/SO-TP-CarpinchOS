#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <lib/matelib.h>
#include <commons/log.h>
#include "main.h"
//------------------------------- PLANIFICACION--------------------//
#include <matelib.h>

#define CANT_CARPINCHOS 2
#define ALLOCAR 10 
#define STRING "0123456789012345678"  

char *LOG_PATH = "./planificacion.log";
char *PROGRAM_NAME = "planificacion";
sem_t *va_el_2;
sem_t *va_el_3;
t_log *logger;

void imprimir_carpincho_n_hace_algo(int numero_de_carpincho)
{
    log_info(logger, "EJECUTANDO Carpincho %d", numero_de_carpincho);
    printf("EJECUTANDO Carpincho %d\n", numero_de_carpincho);
    sleep(2);
}

 void exec_carpincho_1(char *config)
{
    mate_instance self;
    mate_init(&self, config);
    sem_post(va_el_2);
    for (int i = 0; i < 3; i++)
    {
        imprimir_carpincho_n_hace_algo(1);
        mate_call_io(&self, (mate_io_resource) "pelopincho", "Carpincho 1 se va a IO");
    }
    imprimir_carpincho_n_hace_algo(1);
    mate_close(&self);
}

void exec_carpincho_2(char *config)
{
    mate_instance self;
    sem_wait(va_el_2);
    mate_init(&self, config);
    sem_post(va_el_3); //Creo que esta demas, es para que el 3 entre dsp del 2
    for (int i = 0; i < 3; i++)
    {
        imprimir_carpincho_n_hace_algo(2);
        mate_call_io(&self, (mate_io_resource) "pelopincho", "Carpincho 2 se va a IO");
    }
    imprimir_carpincho_n_hace_algo(2);
    mate_close(&self);
}

void exec_carpincho_3(char *config)
{
    mate_instance self;
    sem_wait(va_el_3);
    mate_init(&self, config);
    for (int i = 0; i < 3; i++)
    {
        imprimir_carpincho_n_hace_algo(3);
        imprimir_carpincho_n_hace_algo(3);
        imprimir_carpincho_n_hace_algo(3);
        imprimir_carpincho_n_hace_algo(3);
        imprimir_carpincho_n_hace_algo(3);
        mate_call_io(&self, (mate_io_resource) "pelopincho", "Carpincho 3 se va a IO");
    }
    imprimir_carpincho_n_hace_algo(3);
    imprimir_carpincho_n_hace_algo(3);
    imprimir_carpincho_n_hace_algo(3);
    imprimir_carpincho_n_hace_algo(3);
    imprimir_carpincho_n_hace_algo(3);
    mate_close(&self);
}

void free_all()
{

    sem_destroy(va_el_3);
    free(va_el_3);
    sem_destroy(va_el_2);
    free(va_el_2);

    log_destroy(logger);
}

void init_sems()
{
    va_el_2 = malloc(sizeof(sem_t));
    sem_init(va_el_2, 1, 0);
    va_el_3 = malloc(sizeof(sem_t));
    sem_init(va_el_3, 1, 0);
} 

//----PLANIFICACION MAIN
/*    int main(int argc, char *argv[])
{
    logger = log_create(LOG_PATH, PROGRAM_NAME, true, LOG_LEVEL_DEBUG);
    printf("creo logger\n");
    pthread_t carpincho1_thread;
    pthread_t carpincho2_thread;
    pthread_t carpincho3_thread;
   
 printf("creo hilos\n");
    init_sems();
 printf("creo sems\n");
    pthread_create(&carpincho1_thread, NULL, (void *)exec_carpincho_1, "cfg/carpincho.config");
     printf("creo hilo1\n");
    pthread_create(&carpincho2_thread, NULL, (void *)exec_carpincho_2, "cfg/carpincho.config");
     printf("creo hilo2\n");
    pthread_create(&carpincho3_thread, NULL, (void *)exec_carpincho_3, "cfg/carpincho.config");
     printf("creo hilo3\n");
    pthread_join(carpincho1_thread, NULL);
    pthread_join(carpincho2_thread, NULL);
    pthread_join(carpincho3_thread, NULL);
    free_all();
    puts("Termine!");
}  */


//----------------------SUSPENSION---------------------//
   void* carpincho1_func(void* config){

	mate_instance instance;

	printf("C1 - Llamo a mate_init\n");
	mate_init(&instance, (char*)config);

	printf("C1 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

   mate_pointer mem = mate_memalloc(&instance, ALLOCAR);
   char mens [] = "HOLA";
   mate_memwrite(&instance, mens, mem, strlen(mens));
   void* leido = malloc(strlen(mens)+1);
   mate_memread(&instance, mem, leido, strlen(mens));
   printf("leyo: %s\n", (char*)leido); 
	printf("C1 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");
    mate_memfree(&instance, mem);

	printf("C1 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");
    mate_close(&instance);

	return 0;
}

void* carpincho2_func(void* config){

	mate_instance instance;
    
	printf("C2 - Llamo a mate_init\n");
	mate_init(&instance, (char*)config);

	printf("C2 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

	printf("C2 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

	printf("C2 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");
 printf("Voy a romper todoooooo\n");
    mate_close(&instance);

	return 0;
}

void* carpincho3_func(void* config){

	mate_instance instance;

	printf("C3 - Llamo a mate_init\n");
	mate_init(&instance, (char*)config);

	printf("C3 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

	printf("C3 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

	printf("C3 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

    mate_close(&instance);

	return 0;
}

void* carpincho4_func(void* config){

	mate_instance instance;

	printf("C4 - Llamo a mate_init\n");
	mate_init(&instance, (char*)config);

	printf("C4 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

	printf("C4 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

	printf("C4 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

    mate_close(&instance);

	return 0;
}
 

//----SUSPENSION MAIN
  int main(int argc, char *argv[]) {

   //memoria_carpincho();

	 pthread_t carpincho1;
	pthread_t carpincho2;
	pthread_t carpincho3;
	pthread_t carpincho4;
	
    printf("Prueba de suspensión");
	printf("MAIN - Utilizando el archivo de config: %s\n", "cfg/carpincho.config");

	pthread_create(&carpincho1, NULL, carpincho1_func, "cfg/carpincho.config");
	sleep(1);
 	pthread_create(&carpincho2, NULL, carpincho2_func,"cfg/carpincho.config");
	sleep(1);
   
	pthread_create(&carpincho3, NULL, carpincho3_func, "cfg/carpincho.config");
	sleep(1);
	/*pthread_create(&carpincho4, NULL, carpincho4_func, "cfg/carpincho.config");  */
  
/* 	pthread_join(carpincho4, NULL);*/
	pthread_join(carpincho3, NULL); 
	pthread_join(carpincho2, NULL);  
	pthread_join(carpincho1, NULL);

	printf("MAIN - Retirados los carpinchos de la pelea, hora de analizar los hechos\n");

	return EXIT_SUCCESS;
}  
  
/*
int main(int argc, char* argv[]) {

  // if ( argc>1 && (strcmp(argv[1], "memoria")== 0)) memoria_carpincho();

   pthread_t hilos[CANT_CARPINCHOS];
   printf("entro al main carpincho\n");
   for (int i=0; i < CANT_CARPINCHOS; i++) {
      pthread_create(&hilos[i], NULL, (void*)carpincho_comportamiento, NULL);
   }

   pthread_t hilo;
   pthread_create(&hilo, NULL, (void*)carpincho_comportamiento2, NULL);

   for (int i=0; i < CANT_CARPINCHOS; i++) {
      pthread_join(hilos[i], NULL);
   }
 //pthread_join(hilo, NULL);
   return 0;
}
int main(int argc, char *argv[])
{
    logger = log_create(LOG_PATH, PROGRAM_NAME, true, LOG_LEVEL_DEBUG);
    pthread_t carpincho1_thread;
    pthread_t carpincho2_thread;
    pthread_t carpincho3_thread;

    init_sems();

    pthread_create(&carpincho1_thread, NULL, (void *)exec_carpincho_1, argv[1]);
    pthread_create(&carpincho2_thread, NULL, (void *)exec_carpincho_2, argv[1]);
    pthread_create(&carpincho3_thread, NULL, (void *)exec_carpincho_3, argv[1]);
    pthread_join(carpincho1_thread, NULL);
    pthread_join(carpincho2_thread, NULL);
    pthread_join(carpincho3_thread, NULL);
    free_all();
    puts("Termine!");
} */

 void carpincho_comportamiento_memoria(void* arg)
{
   mate_instance* c = malloc(sizeof(mate_instance));
   mate_inner_structure* info = (mate_inner_structure*)c;

   printf("-- Mate INIT -- \n");
   int init = mate_init(c, "cfg/carpincho.config");
   if (init == -1){
      fprintf(stderr, "No se puedo iniciar el carpincho \n");
   }

   printf("-- Mate MALLOC -- \n");
   int dl = mate_memalloc(c, ALLOCAR);
   if (dl == -1){
      fprintf(stderr, "No pude reservar memoria \n");
	  mate_close(c);
	  pthread_exit(0);
   }
   else{
	   printf("\n La direccion logica es: %d \n", dl);
   }

   //int dl_ = mate_memalloc(c, 14);
   //mate_memfree(c, dl_);

   printf("-- Mate WRITE -- \n");
   char* saturno = STRING;
   int size = strlen(saturno) + 1;
   void* buffer = malloc(size);
   memcpy(buffer, saturno, size);

   int escritos = mate_memwrite(c, buffer, dl , size);
   if (escritos == MATE_WRITE_FAULT){
      fprintf(stderr, "Error de MATE_WRITE_FAULT \n");
      abort();
   }

   printf("-- Mate READ -- \n");
   void* leer = malloc(size);
   int leido = mate_memread(c, dl, leer,  size);
   if (leido == MATE_READ_FAULT) {
      fprintf(stderr, "Error de MATE_READ_FAULT");
      abort();
   }
	printf("MemRead leyo: %s\n", (char*)leer);


   printf("-- Mate FREE -- \n");
   int free_valido = mate_memfree(c, dl);
   if (free_valido == MATE_FREE_FAULT){
     fprintf(stderr, "Error de MATE_FREE_FAULT \n");
     //abort();
   }

   printf("-- Mate CLOSE -- \n");
   int fin = mate_close(c);
   if (fin != 0){
      fprintf(stderr, "El programa termino con errores");
      abort();
   }

   printf("Termino memoria\n");
   free(buffer);
}

void memoria_carpincho() 
{
   printf("Voy a probar carpincho solo con memoria\n");
   pthread_t hilos[CANT_CARPINCHOS];
   for (int i=0; i < CANT_CARPINCHOS; i++) {
      pthread_create(&hilos[i], NULL, (void*)carpincho_comportamiento_memoria, NULL);
   }

   for (int i=0; i < CANT_CARPINCHOS; i++) {
      pthread_join(hilos[i], NULL);
   }

   abort();
} 

/* //------------------------------- DEADLOCK--------------------//


void* carpincho1_func(void* config){

    mate_instance instance;

    mate_init(&instance, config);

    printf("C1 - Toma SEM1\n");
    mate_sem_wait(&instance, "SEM1");
    sleep(3);
    printf("C1 - Toma SEM2\n");
    mate_sem_wait(&instance, "SEM2");
    sleep(3);

    printf("C1 - libera SEM1\n");
    mate_sem_post(&instance, "SEM1");
    printf("C1 - libera SEM2\n");
    mate_sem_post(&instance, "SEM2");

    printf("C1 - Se retira a descansar\n");
    mate_close(&instance);
    return 0;
}

void* carpincho2_func(void* config){

    mate_instance instance;

    mate_init(&instance, config);

    printf("C2 - toma SEM2\n");
    mate_sem_wait(&instance, "SEM2");
    sleep(3);
    printf("C2 - toma SEM3\n");
    mate_sem_wait(&instance, "SEM3");
    sleep(3);

    printf("C2 - libera SEM2\n");
    mate_sem_post(&instance, "SEM2");
    printf("C2 - libera SEM3\n");
    mate_sem_post(&instance, "SEM3");

	printf("C2 - Se retira a descansar\n");
	mate_close(&instance);
	return 0;
}

void* carpincho3_func(void* config){

    mate_instance instance;

    mate_init(&instance, config);

    printf("C3 - toma SEM3\n");
    mate_sem_wait(&instance, "SEM3");
    sleep(3);
    printf("C3 - toma SEM4\n");
    mate_sem_wait(&instance, "SEM4");
     printf("dormire 3 SEGNDOS\n");
    sleep(3);
 printf("PASARON LOS 3 SEGNDOS carpincho 3\n");
    printf("C3 - libera SEM3\n");
    mate_sem_post(&instance, "SEM3");
    printf("C3 - libera SEM4\n");
    mate_sem_post(&instance, "SEM4");

	printf("C3 - Se retira a descansar\n");
	mate_close(&instance);
	return 0;
}

void* carpincho4_func(void* config){

    mate_instance instance;

    mate_init(&instance, config);

    printf("C4 - toma SEM4\n");
    mate_sem_wait(&instance, "SEM4");
    sleep(3);
    printf("C4 - toma SEM1\n");
    mate_sem_wait(&instance, "SEM1");
    sleep(3);
    printf("PASARON LOS 3 SEGNDOS DESPUES DE RECIBIR -1 DEL KERNEL\n");
 printf("C4 - libera SEM1\n");
    mate_sem_post(&instance, "SEM1");
    printf("C4 - libera SEM4\n");
    mate_sem_post(&instance, "SEM4");
 
	printf("C4 - Se retira a descansar\n");
	mate_close(&instance);
	return 0;
}

void* carpincho5_func(void* config){

    mate_instance instance;

    mate_init(&instance, config);

    printf("C5 - toma SEM5\n");
    mate_sem_wait(&instance, "SEM5");
    sleep(3);
    printf("C5 - toma SEM6\n");
    mate_sem_wait(&instance, "SEM6");
    sleep(3);

    printf("C5 - toma SEM5\n");
    mate_sem_post(&instance, "SEM5");
    printf("C5 - toma SEM6\n");
    mate_sem_post(&instance, "SEM6");

	printf("C5 - Se retira a descansar\n");
	mate_close(&instance);
    printf("C5 - Se retira HIZO MATE CLOSE\n");
	return 0;
}

void* carpincho6_func(void* config){

    mate_instance instance;

    mate_init(&instance, config);

    printf("C6 - toma SEM6\n");
    mate_sem_wait(&instance, "SEM6");
    sleep(3);
    printf("C6 - toma SEM1\n");
    mate_sem_wait(&instance, "SEM1");
    sleep(3);
    printf("C6 - Libera SEM1\n");
    mate_sem_post(&instance, "SEM1");
 printf("se resolvio primer deadlock quedara esperando en SEM5\n");
    printf("C6 - toma SEM5\n");
    mate_sem_wait(&instance, "SEM5");
    sleep(3);

     mate_sem_post(&instance, "SEM5");
    mate_sem_post(&instance, "SEM6");
 
	printf("C6 - Se retira a descansar\n");
	mate_close(&instance);
	return 0;
} 
//------DEADLOCK MAIN
int main(int argc, char *argv[]) {

    if ( argc>1 && (strcmp(argv[1], "memoria")== 0)) memoria_carpincho();

    mate_instance instance;

    mate_init(&instance, "cfg/carpincho.config");

    printf("paso init\n");

  // Creamos los semaforos que van a usar los carpinchos
    mate_sem_init(&instance, "SEM1", 1);
    mate_sem_init(&instance, "SEM2", 1);
    mate_sem_init(&instance, "SEM3", 1);
    mate_sem_init(&instance, "SEM4", 1);
    mate_sem_init(&instance, "SEM5", 1);
    mate_sem_init(&instance, "SEM6", 1);

  // Deadlock entre estos 4
	pthread_t carpincho1;
	pthread_t carpincho2;
	pthread_t carpincho3;
	pthread_t carpincho4;

  // Deadlock entre estos 2 con uno pendiente del anterior
	pthread_t carpincho5;
	pthread_t carpincho6;


	printf("MAIN - Utilizando el archivo de config: %s\n", "cfg/carpincho.config");

	pthread_create(&carpincho1, NULL, carpincho1_func, "cfg/carpincho.config");
    sleep(1);
	pthread_create(&carpincho2, NULL, carpincho2_func, "cfg/carpincho.config");
    sleep(1);
	pthread_create(&carpincho3, NULL, carpincho3_func, "cfg/carpincho.config");
    sleep(1);
	pthread_create(&carpincho4, NULL, carpincho4_func, "cfg/carpincho.config");
    sleep(1);
	pthread_create(&carpincho5, NULL, carpincho5_func, "cfg/carpincho.config");
    sleep(1);
	pthread_create(&carpincho6, NULL, carpincho6_func, "cfg/carpincho.config");
    sleep(1);

    mate_close(&instance);

	pthread_join(carpincho6, NULL);
	pthread_join(carpincho5, NULL);
	pthread_join(carpincho4, NULL);
	pthread_join(carpincho3, NULL);
	pthread_join(carpincho2, NULL);
	pthread_join(carpincho1, NULL);
  
	printf("MAIN - Como no sabemos a quienes va a matar el algoritmo, entonces hacemos el free de los semáforos acá");
	mate_init(&instance, "cfg/carpincho.config");
    mate_sem_destroy(&instance, "SEM1");
    mate_sem_destroy(&instance, "SEM2");
    mate_sem_destroy(&instance, "SEM3");
    mate_sem_destroy(&instance, "SEM4");
    mate_sem_destroy(&instance, "SEM5");
    mate_sem_destroy(&instance, "SEM6");
	mate_close(&instance);

	printf("MAIN - Retirados los carpinchos de la pelea, hora de analizar los hechos\n");

	return EXIT_SUCCESS;
}  
 
 */