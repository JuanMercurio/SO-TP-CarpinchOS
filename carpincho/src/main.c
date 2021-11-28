#include "main.h"

#include <matelib/matelib.h>
#include <pthread.h>

#define CANT_CARPINCHOS 1

int main(int argc, char* argv[]) {

   pthread_t hilos[CANT_CARPINCHOS];
   printf("entro al main carpincho\n");
   for(int i=0; i < CANT_CARPINCHOS; i++) {
      pthread_create(&hilos[i], NULL, (void*)carpincho_comportamiento, NULL);
   }

   for(int i=0; i < CANT_CARPINCHOS; i++) {
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
   devolvio = mate_sem_init(ref, "SEM_HELLO", 0);
    printf("devolvio %d\n", devolvio);
   sleep(2);
      printf("VOY A cerrar instancia\n");
   mate_close(ref);

   printf("Termine\n");
//   mate_pointer dl = mate_memalloc(ref, 100);

}