#include "main.h"

#include <matelib/matelib.h>

int main(int argc, char* argv[]) {

   mate_instance* ref = malloc(sizeof(mate_instance));

   mate_init(ref, "cfg/carpincho.config");

   mate_pointer dl = mate_memalloc(ref, 100);

   return 0;
}

