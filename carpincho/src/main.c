#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <lib/matelib.h>
#include <commons/log.h>
#include "main.h"
#include <matelib/matelib.h>

#define CANT_CARPINCHOS 1
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

void* carpincho1_func(void* config){

	mate_instance instance;

	printf("C1 - Llamo a mate_init\n");
	mate_init(&instance, (char*)config);

	printf("C1 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

	printf("C1 - Hace una llamada a IO\n");
	mate_call_io(&instance, "PILETA", "Vamos a usar la pileta...");

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


int main(int argc, char *argv[]) {

	pthread_t carpincho1;
	pthread_t carpincho2;
	pthread_t carpincho3;
	pthread_t carpincho4;

	printf("MAIN - Utilizando el archivo de config: %s\n", "cfg/carpincho.config");

	pthread_create(&carpincho1, NULL, carpincho1_func,"cfg/carpincho.config");
	sleep(1);
	pthread_create(&carpincho2, NULL, carpincho2_func, "cfg/carpincho.config");
	sleep(1);
	pthread_create(&carpincho3, NULL, carpincho3_func,"cfg/carpincho.config");
	sleep(1);
	pthread_create(&carpincho4, NULL, carpincho4_func, "cfg/carpincho.config");

	pthread_join(carpincho4, NULL);
	pthread_join(carpincho3, NULL);
	pthread_join(carpincho2, NULL);
	pthread_join(carpincho1, NULL);

	printf("MAIN - Retirados los carpinchos de la pelea, hora de analizar los hechos\n");

	return EXIT_SUCCESS;
}

/*int main(int argc, char *argv[])
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
} */
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
/*int main(int argc, char *argv[])
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
void carpincho_comportamiento(void *arg)
{
   mate_instance *ref = malloc(sizeof(mate_instance));
   printf("creo instancia\n");
   int devolvio = mate_init(ref, "cfg/carpincho.config");
   printf("devolvio %d\n", devolvio);
/*
   printf("VOY A MANADAR UN SEM INIT\n");
   devolvio = mate_sem_init(ref, "SEM_HELLO", 0);

   printf("devolvio el sem_init: %d\n", devolvio);
   devolvio = mate_sem_init(ref, "SEM_BYE", 0);

   printf("devolvio el sem_init: %d\n", devolvio);
   printf("QUEDO TRABADO EN WAIT\n");

   devolvio = mate_sem_wait(ref, "SEM_HELLO");
   printf("devolvio sem wait: %d\n", devolvio);
   printf(" PASO WAIT\n");


   devolvio = mate_sem_destroy(ref, "SEM_HELLO");
   printf("devolvio el sem_destroy: %d\n", devolvio);
sleep(5);
      devolvio = mate_sem_post(ref, "SEM_BYE");
   printf("devolvio sem post: %d\n", devolvio);
   */
sleep(2); //procesador
 devolvio = mate_call_io(ref, "hierbitas", "hi");// seva a io
   printf("TERMINO IO: hierbitas carpincho %d\n", ((mate_inner_structure *)ref->group_info)->pid);
sleep(2);//porcesador
 devolvio =  mate_call_io(ref, "laguna", "hi");
 printf("TERMINO IO: laguna carpincho %d con %d\n", ((mate_inner_structure *)ref->group_info)->pid, devolvio);
sleep(2);
   printf("VOY A cerrar instancia\n");
   mate_close(ref);

   printf("-------------------------------------Termine\n");
   //   mate_pointer dl = mate_memalloc(ref, 100);
}
void carpincho_comportamiento2(void* arg){
  mate_instance* ref = malloc(sizeof(mate_instance));
    printf("creo instancia\n");
   int devolvio = mate_init(ref, "cfg/carpincho.config");
   printf("devolvio -2 %d\n", devolvio);
   sleep(1);

 devolvio = mate_call_io(ref, "hierbitas","nada");
  printf("devolvio call io -2: %d\n", devolvio);
  
       devolvio =mate_sem_post(ref, "SEM_HELLO");
    printf("devolvio sem post -2: %d\n", devolvio);

       devolvio =mate_sem_wait(ref, "SEM_BYE");
    printf("devolvio sem wait -2: %d\n", devolvio);

       devolvio =mate_sem_destroy(ref, "SEM_BYE");
    printf("devolvio sem destroy -2: %d\n", devolvio);

      printf("===========================0VOY A cerrar instancia\n");
   mate_close(ref);

   printf("=======================================0Termine\n");
}
void carpincho_comportamiento3(void *arg)
{
   mate_instance* ref = malloc(sizeof(mate_instance));
    printf("creo instancia\n");
   int devolvio = mate_init(ref, "cfg/carpincho.config");
   printf("devolvio -2 %d\n", devolvio);
   sleep(1);
   char * mensaje = "hola";
   devolvio = mate_memwrite(ref,mensaje, 1234, strlen(mensaje));
  printf("devolvio memwrite -2 %d\n", devolvio);
   mate_close(ref);

}

void carpincho_comportamiento_memoria(void* arg)
{
   mate_instance* c = malloc(sizeof(mate_instance));

   printf("-- Mate INIT -- \n");
   int init = mate_init(c, "cfg/carpincho.config");
   if (init == -1){
      fprintf(stderr, "No se puedo iniciar el carpincho \n");
   }

   printf("-- Mate MALLOC -- \n");
   int dl = mate_memalloc(c, 117);
   if (dl == -1){
      fprintf(stderr, "No pude reservar memoria \n");
      abort();
   }
   else{
	   printf("\n La direccion logica es: %d \n", dl);
   }

   printf("-- Mate WRITE -- \n");
   char* saturno = "jejeje";
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

sleep(1);

   printf("-- Mate FREE -- \n");
   int free_valido = mate_memfree(c, dl);
   if (free_valido == MATE_FREE_FAULT){
     fprintf(stderr, "Error de MATE_FREE_FAULT \n");
     abort();
   }

   printf("-- Mate CLOSE -- \n");
   int fin = mate_close(c);
   if (fin != 0){
      fprintf(stderr, "El programa termino con errores");
      abort();
   }
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
