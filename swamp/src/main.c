#include "main.h"

int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    

   obtener_config();

   terminar_programa();

   return 0;
}


void terminar_programa(){
   config_destroy(config);
}