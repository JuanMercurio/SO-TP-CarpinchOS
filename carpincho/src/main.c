#include "main.h"

#include <matelib/matelib.h>
#include <pthread.h>

#define CANT_CARPINCHOS 1

int main(int argc, char* argv[]) {
  // if ( argc>=1 && (strcmp(argv[1], "memoria")== 0)) memoria_carpincho();

   pthread_t hilos[CANT_CARPINCHOS];
   printf("entro al main carpincho\n");
   for (int i=0; i < CANT_CARPINCHOS; i++) {
      pthread_create(&hilos[i], NULL, (void*)carpincho_comportamiento, NULL);
   }

   for (int i=0; i < CANT_CARPINCHOS; i++) {
      pthread_join(hilos[i], NULL);
   }

   return 0;
}

void carpincho_comportamiento(void* arg)
{
   mate_instance* ref = malloc(sizeof(mate_instance));
    printf("creo instancia\n");
   int devolvio = mate_init(ref, "cfg/carpincho.config");
   printf("devolvio %d\n", devolvio);
   
   sleep(1);
   printf("VOY A MANADAR UN SEM INIT\n");
   devolvio = mate_sem_init(ref, "SEM_HELLO", 1);
    printf("devolvio el sem_init: %d\n", devolvio);
     devolvio = mate_sem_init(ref, "SEM_HELLO", 1);
    printf("devolvio el sem_init: %d\n", devolvio);
   sleep(1);
      devolvio =mate_sem_wait(ref, "SEM_HELLO");
    printf("devolvio sem wait: %d\n", devolvio);
   devolvio =mate_sem_wait(ref, "SEM_HEL");

 devolvio = mate_call_io(ref, "hierbitas","nada");
  printf("devolvio call io: %d\n", devolvio);
  
       devolvio =mate_sem_wait(ref, "SEM_HELLO");
    printf("devolvio sem wait: %d\n", devolvio);


  devolvio = mate_sem_destroy(ref, "SEM_HELLO");
  printf("devolvio el sem_destroy: %d\n", devolvio);
      printf("VOY A cerrar instancia\n");
   mate_close(ref);

   printf("Termine\n");
//   mate_pointer dl = mate_memalloc(ref, 100);

}

void carpincho_comportamiento_memoria(void* arg)
{
   mate_instance* c = malloc(sizeof(mate_instance));

   int init = mate_init(c, "cfg/carpincho.config");
   if (init == -1){
      fprintf(stderr, "No se puedo iniciar el carpincho \n");
   }

   int dl = mate_memalloc(c, 100);
   if (dl == -1){
      fprintf(stderr, "No pude reservar memoria \n");
   }

   dl = mate_memfree(c, 10101010);
   if (dl == MATE_FREE_FAULT){
     fprintf(stderr, "Error de MATE_FREE_FAULT \n");
   }

   int i = 400;
   void* buffer = malloc(sizeof(int));
   memcpy(buffer, &i, sizeof(int));

   dl = mate_memwrite(c, buffer, 100,sizeof(int));
   if (dl == MATE_WRITE_FAULT){
      fprintf(stderr, "Error de MATE_WRITE_FAULT \n");
   }

   dl = mate_memread(c, 1010, buffer, 100);
   if (dl == MATE_READ_FAULT){
      fprintf(stderr, "Error de MATE_READ_FAULT \n");
   }

   int fin = mate_close(c);
   if (fin != 0){
      fprintf(stderr, "El programa termino con errores");
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