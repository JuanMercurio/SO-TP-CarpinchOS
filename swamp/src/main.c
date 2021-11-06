#include "main.h"



int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    

   /* Este modulo funciona sin hilos. Los clientes esperan a ser atendidos en cola  */
   obtener_config();
   
   printf("El ip del server va a ser: %s \n", configuracion.IP);   
   printf("El ip del server va a ser: %s \n", configuracion.PUERTO);   
   printf("El ip del server va a ser: %d \n", configuracion.TAMANIO_SWAP);   
   printf("El ip del server va a ser: %d \n", configuracion.TAMANIO_PAGINA);   
   printf("El ip del server va a ser: %d \n", configuracion.RETARDO_SWAP);  
   printf("Los archivos del server van a ser: %s \n", configuracion.ARCHIVOS_SWAP_list[0]);  
   printf("Los archivos del server van a ser: %s \n", configuracion.ARCHIVOS_SWAP_list[1]);
   printf("Los archivos del server van a ser: %s \n", configuracion.ARCHIVOS_SWAP_list[2]);   
    //printf("La cantidad de elementos %d \n", lenght(configuracion.ARCHIVOS_SWAP_list));   
   
   crearArchivos();
   /*iniciar_swamp();
   atender_clientes();
   terminar_programa();*/

   return 0;
}

void crearArchivos(){
    // crea archivos
    int file = open (configuracion.ARCHIVOS_SWAP_list[0] , 0);
    file = open (configuracion.ARCHIVOS_SWAP_list[0] , 2);
    FILE* archivo = fopen (configuracion.ARCHIVOS_SWAP_list[0],"w");
    int truncate = ftruncate (archivo, 8192*2 );
    int trun2 = ftruncate(file,8192*2);
    //truncate(configuracion.ARCHIVOS_SWAP_list[0],8192);
    struct stat* estadisticas;
    //int tamano = fstat (archivo,estadisticas);
    //int tam = stat (configuracion.ARCHIVOS_SWAP_list[0], estadisticas);
    printf("El valor del file es %d\n",file);
    printf("El valor de archivo es %d\n",archivo);
    printf("El valor de truncate es %d\n",truncate);
    printf("El valor de truncate es %d\n",trun2);
    // printf("El valor de trun es %d\n",trun);
   // printf("El valor de truncate es %d\n",tamano);
    //printf("El valor del file es %d\n",tam);
    //printf("El valor el tamaño es %d\n",estadisticas.st_size);
    fputs("/0",archivo);
    fclose(archivo);
    //printf("El valor del file es %d", *archivo);
    //printf("EL valor del truncate es %s",truncate);
}

// hacer funcion que escriba un carpincho una pagina
// hacer que lea una pagina
// busqueda de pagina por carpincho.


void terminar_programa(){
   config_destroy(config);
}

void iniciar_swamp(){
   server = iniciar_servidor(configuracion.IP, configuracion.PUERTO);
}

void atender_clientes(){ 

   // while feo
   while(1){ 
      int cliente = aceptar_cliente(server);
      printf("Se conecto alguien en el socket: %d\n", cliente);
   }
}
