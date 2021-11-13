#include "main.h"
#include <mensajes/mensajes.h>

#define VALIDO 0
#define INVALIDO -1


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
    asignacionFija = 1;
    carpinchosEnArchivo= list_create();
    crearArchivos();
   //int numMejor= elegirMejorArchivo();
    crearCarpincho (5,2,"pagina1");
    crearCarpincho (6,6,"pagina2");
    crearCarpincho (7,2,"pagina2");
    char* basura = string_repeat('n',configuracion.TAMANIO_PAGINA);
    int pudo=remplazoPaginaFija(2,2,basura);

    //crearCarpincho (7);
   /*iniciar_swamp();
   terminar_programa();*/
   /*iniciar_swamp();
   atender_clientes();*/
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
        
        //printf("El valor de truncate es %d\n",tamano);
         //printf("el valor de i es %d.\n",i);
        i++;
       
    }
     //printf("FIN aapertura archivo y truncate\n");
        
}
int elegirMejorArchivo(){
    // busca el archivo con el que mas espacio tenga. 
    int i = 0;
    int menor = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i]);
    int indiceMenor = 0;
   
   //int menor2 = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[1]);
   //printf("valor menor %d\n",menor);
    //printf("valor menor2 %d\n",menor2);
    while(configuracion.ARCHIVOS_SWAP_list[i]){
       //printf("el valor de i es %d.\n",i);
        if (i == 0){
           menor = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i]);

        }
        else if (menor > cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i])){
            menor = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i]);
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
    printf("el indice menor es %d.\n",indiceMenor);
    return indiceMenor;
}
void crearCarpincho (int pidd, int pag, char* contenidoPagina){
    int mejorArchivo = elegirMejorArchivo();
    int file =  open(configuracion.ARCHIVOS_SWAP_list[mejorArchivo], O_RDWR,S_IRUSR|S_IWUSR, O_APPEND);
    Carpincho_Swamp* carpincho;
    carpincho = malloc(sizeof(Carpincho_Swamp));
    carpincho->pid = pidd;
    carpincho->numeroArchivo = mejorArchivo;
    list_add(carpinchosEnArchivo,carpincho);
    if( cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[mejorArchivo]) != 2){
         lseek (file , cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[mejorArchivo]), 0);
    }
    if(asignacionFija){
        char* basura = string_repeat('-',configuracion.TAMANIO_PAGINA);
        for(int j = 0; j< configuracion.MARCOS_MAXIMOS; j++){
            write(file,basura,configuracion.TAMANIO_PAGINA);
        }
    }
    close(file);
}
int remplazoPaginaFija(int pid, int pagina, char*contenido){
    int mejorArchivo = buscarArchivoDeCarpincho (pid);
   
      printf("ENTRO ACA1\n");
    if(  mejorArchivo != -1 ){
        printf("ENTRO ACA2\n");
        int  file =  open(configuracion.ARCHIVOS_SWAP_list[mejorArchivo], O_RDWR, S_IRUSR|S_IWUSR);
        struct stat sb;
        
        char * file_in_memory = mmap(NULL,cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[mejorArchivo]),PROT_READ |PROT_WRITE ,MAP_SHARED, file,0);
       
        for (int i = (pagina-1)*configuracion.TAMANIO_PAGINA ;i<pagina*configuracion.TAMANIO_PAGINA;i++){
            file_in_memory[i]='j';
        }
        printf("\n");
        munmap(file_in_memory,cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[mejorArchivo]));
        close(file);
        printf("la cantidad es %d.\n",cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[mejorArchivo]));
        return 1;
    }
    else{
        printf("NO SE ENCONTRO LA PAGINA");
        return -1;
    }
    
}
int buscarArchivoDeCarpincho (int pidd){
    int max = list_size(carpinchosEnArchivo);
    Carpincho_Swamp* car = malloc(sizeof(Carpincho_Swamp));
    printf("el max es %d\n",max);
    for (int i = 0;i<max;i++){
        
        car = (Carpincho_Swamp*) list_get(carpinchosEnArchivo,i);
        printf("EL pid es %d \n",car->pid);
        if(car->pid == pidd){
            printf("EL numero de archivo es %d \n",car->numeroArchivo);
            int devuelve =car->numeroArchivo;
            free(car);
            return devuelve;
        }
    }
    return -1;
}
int cantidadCaracteresFile (char* path){
    FILE* archivo = fopen (path , "r");
    
    char bufer[configuracion.TAMANIO_SWAP];
    char* ptr = fgets(bufer, configuracion.TAMANIO_SWAP, archivo);
    int cant = string_length(ptr);
    //printf("El valor del archivo es %s\n",bufer);
    //printf("La cantidad es %d\n",cant);
    fclose(archivo);
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
      //printf("Se conecto alguien en el socket: %d\n", cliente);
       int operacion = recibir_operacion(cliente);
      int estado;
      switch (operacion)
      {
      case SOLICITUD_INICIO:
      // estado = funcion que determina si puede iniciar un nuevo proceso (0 si puede, -1 si no puede)
         estado = -1;
         resolver_estado(estado, cliente);
         break;


      //faltan las demas operaciones
      default:
         break;
      }
   }
}

int resolver_estado(int estado, int cliente){

   if (estado == INVALIDO)
   {
      enviar_int(cliente, INVALIDO);
      return INVALIDO;
   }

   if (estado == VALIDO)
   {
      enviar_int(cliente, VALIDO);
      return recibir_int(cliente);
   }

}
