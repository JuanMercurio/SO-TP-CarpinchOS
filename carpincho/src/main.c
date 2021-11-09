#include "main.h"

#include <matelib/matelib.h>

int main(int argc, char* argv[]) {

   mate_instance* ref = malloc(sizeof(mate_instance));
   mate_init(ref, "cfg/carpincho.config");
   int pid = obtener_int(ref, PID);
   printf("Me asignaron el PID: %d", pid);

   return 0;
}

