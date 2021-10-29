#include "main.h"

#include <matelib/matelib.h>

int main(int argc, char* argv[]) {

   mate_instance* ref = malloc(sizeof(mate_instance));
   mate_init(ref, "../../carpincho.config");
   printf("Me pusieron el pid: %d\n", ref->pid);

   return 0;
}

