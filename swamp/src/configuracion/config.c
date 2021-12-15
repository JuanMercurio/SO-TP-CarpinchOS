#include "config.h"

t_config* config;
t_cfg configuracion;

void obtener_config(){
    char* path = "./cfg/swamp.config";
   FILE *fp;
   int c;
  
   fp = fopen(path,"r");
   while(1) {
      c = fgetc(fp);
      if( feof(fp) ) { 
         break ;
      }
      printf("%c", c);
   }
   printf("\n");
   fclose(fp);
   printf("EMPIEZO A LEER LA CONFIG\n");
    config = config_create(path);
    printf("IP: \n");
    configuracion.IP = config_get_string_value(config, "IP");
    printf("ARCHIVOS_SWAMP \n");
    configuracion.ARCHIVOS_SWAP = config_get_string_value(config, "ARCHIVOS_SWAP");
    printf("ACHIVOS SWAMP\n");
    configuracion.ARCHIVOS_SWAP_list = config_get_array_value(config,"ARCHIVOS_SWAP");
    printf("PUERTO \n");
    configuracion.PUERTO = config_get_string_value(config, "PUERTO");
    printf("MARCOS_MAXIMOS \n");
    configuracion.MARCOS_MAXIMOS = config_get_int_value(config, "MARCOS_MAXIMOS");
    printf("RETARDO\n");
    configuracion.RETARDO_SWAP = config_get_int_value(config, "RETARDO_SWAP");
    printf("TAMAÑO PAGINA\n");
    configuracion.TAMANIO_PAGINA = config_get_int_value(config, "TAMANIO_PAGINA");
    printf("TAMANIO SWAP\n");
    configuracion.TAMANIO_SWAP = config_get_int_value(config, "TAMANIO_SWAP");
}
