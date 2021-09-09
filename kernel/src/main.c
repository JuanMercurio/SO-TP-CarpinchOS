#include "main.h"

int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    
   
   while(1){
      helloWorld();
      sleep(2);
   }
   return 0;
}
