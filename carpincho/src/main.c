#include "main.h"

#include <matelib/matelib.h>

int main(int argc, char* argv[]) {

   mate_instance* ref = malloc(sizeof(mate_instance));
   mate_init(ref, "../../carpincho.config");
   int pid = obtener_int(ref, PID);
   printf("Na posta me llego este pid: %d", pid);

   return 0;
}

