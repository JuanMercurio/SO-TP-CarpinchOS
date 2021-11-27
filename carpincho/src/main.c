#include "main.h"

#include <matelib/matelib.h>
#include <pthread.h>

#define CANT_CARPINCHOS 10

int main(int argc, char* argv[]) {

   pthread_t hilos[CANT_CARPINCHOS];

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
   mate_init(ref, "cfg/carpincho.config");

   int pid =  ((mate_inner_structure*)ref->group_info)->pid;
   printf("Termine con el pid %d\n", pid);

//   mate_pointer dl = mate_memalloc(ref, 100);

}