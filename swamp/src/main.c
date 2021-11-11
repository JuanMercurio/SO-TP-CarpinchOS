#include "main.h"



int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    

   /* Este modulo funciona sin hilos. Los clientes esperan a ser atendidos en cola  */
   obtener_config();
   
  /* printf("El ip del server va a ser: %s \n", configuracion.IP);   
   printf("El ip del server va a ser: %s \n", configuracion.PUERTO);   
   printf("El ip del server va a ser: %d \n", configuracion.TAMANIO_SWAP);   
   printf("El ip del server va a ser: %d \n", configuracion.TAMANIO_PAGINA);   
   printf("El ip del server va a ser: %d \n", configuracion.RETARDO_SWAP);  
   printf("Los archivos del server van a ser: %s \n", configuracion.ARCHIVOS_SWAP_list[0]);  
   printf("Los archivos del server van a ser: %s \n", configuracion.ARCHIVOS_SWAP_list[1]);
  */
    //printf("La cantidad de elementos %d \n", lenght(configuracion.ARCHIVOS_SWAP_list));   
   
   //crearArchivos();
   
   /*int num2 =  cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[0]);
   int num3 =  cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[1]);
   printf("el num 2 es %d\n",num2);
   printf("el num 3 es %d\n",num3);*/

   
    crearCarpincho (5,0);
    crearCarpincho (6,1);
    crearCarpincho (5,0);
    crearCarpincho (6,1);
    //crearCarpincho (7);
   /*iniciar_swamp();
   atender_clientes();
   terminar_programa();*/

   return 0;
}

void crearArchivos(){
    
    int i = 0;
    while(configuracion.ARCHIVOS_SWAP_list[i]){
        // intenta abrirlo apra escritura, si no existe lo crea
        if (fopen (configuracion.ARCHIVOS_SWAP_list[i],"r") == -1){
                printf("EL archivo no existe. \n");
        }
        FILE* archivo = fopen (configuracion.ARCHIVOS_SWAP_list[i],"a");
        fputs("/0",archivo);
        fclose(archivo);
        // abre el archivo en modo escritura
        int file = open (configuracion.ARCHIVOS_SWAP_list[i] , 2);
        // le trunca (coloca) un espacio fijo. puede que tenga datos y lo borre
        int trun2 = ftruncate(file,configuracion.TAMANIO_SWAP);
        struct stat statbuf;
        int size = fstat(file,&statbuf);
        close(file);
        int tamano = statbuf.st_size;
        
        printf("El valor de truncate es %d\n",tamano);
         printf("el valor de i es %d.\n",i);
        i++;
       
    }
     printf("FIN aapertura archivo y truncate\n");
        
}
int elegirMejorArchivo(){
    // busca el archivo con el que mas espacio tenga. 
    int i = 0;
    int menor = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i]);
    int indiceMenor = 0;
   int menor2 = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[1]);
   printf("valor menor %d\n",menor);
    printf("valor menor2 %d\n",menor2);
    while(configuracion.ARCHIVOS_SWAP_list[i]){
         i++;
         
       //printf("ENtro al while\n");
       int otro = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i]);
       if (menor > otro){
           //printf("el valor de i es %d.\n",i);
           //printf("el menor es %s.\n",menor);
            menor = otro;
           // printf("valor menor %d\n",menor);
            indiceMenor = i;
        }
        // FUNCION STAT NO USADA
        /*int file = open (configuracion.ARCHIVOS_SWAP_list[i] , 1);
        struct stat statbuf;
        int size = fstat(file,&statbuf);
        close(file);
        int tamano = statbuf.st_size; 
        printf("El valor de truncate es %d\n",tamano);
        */
        
	    //char** list = string_split(bufer, "\n");
         /* if (ptr == NULL) {
                printf("es null \n");
            }*/
        //printf("El valor del archivo es %s\n",devuelve);
        i++;
    }
    //printf("el indice menor es %d.\n",indiceMenor);
    return indiceMenor;
}
void crearCarpincho (int pid,int mejorArchivo){
    mejorArchivo = elegirMejorArchivo();
    printf("el mejor archivo es %d",mejorArchivo);
    FILE* archivo = fopen (configuracion.ARCHIVOS_SWAP_list[mejorArchivo] , "r+");
    /*int cant = sizeof("hoola");
     printf("La cantidad de hoola es %d\n",cant);*/
    //fwrite( pid, sizeof(int), sizeof(pid), archivo );
    fprintf(archivo, "%d",5);
    fprintf(archivo, "%s","chau|");
    fprintf(archivo, "%s","send|\n");
    //fputs(5,archivo);
    //lfputs("hola",archivo);
    fclose(archivo);
    printf("FIN CREAR\n");
}
int cantidadCaracteresFile (char* path){
    FILE* archivo = fopen (path , "r");
    
    char bufer[configuracion.TAMANIO_SWAP];
    //char* ptr = fgets(bufer, configuracion.TAMANIO_SWAP, archivo);
    int i = 0;
    char caracter[configuracion.TAMANIO_SWAP];
    caracter[i] = fgetc(archivo);
    while(caracter[i] != EOF){
        i++;
        caracter[i] = fgetc(archivo);
    }
    //printf("El valor del archivo es %s.\n",caracter);
    int cant = string_length(caracter);
    printf("El path es %s.\n",path);
    //printf("El valor del archivo es %s.\n",bufer);
    printf("La cantidad es %d.\n",cant);
    
    return cant;
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
