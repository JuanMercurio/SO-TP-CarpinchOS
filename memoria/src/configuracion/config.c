#include "config.h"

#include <pthread.h>

t_log* logger_memoria;
pthread_mutex_t mutex_log;

t_config* config;
t_cfg configuracion;

void obtener_config(){

    config = config_create("cfg/memoria.config");

    if (!config_has_property(config, "IP"))                       printf("El valor IP no existe en la config\n");
    if (!config_has_property(config, "PUERTO"))                   printf(" El valorPUERTO no existe en la config\n");
    if (!config_has_property(config, "TAMANIO"))                  printf(" El valor TAMANIO no existe en la config\n");
    if (!config_has_property(config, "TAMANIO_PAGINA"))           printf(" El valor TAMANIO_PAGINA no existe en la config\n");
    if (!config_has_property(config, "TIPO_ASIGNACION"))          printf(" El valor TIPO_ASIGNACION no existe en la config\n");
    if (!config_has_property(config, "MARCOS_POR_CARPINCHO"))     printf(" El valor MARCOS_POR_CARPINCHO no existe en la config\n");
    if (!config_has_property(config, "ALGORITMO_REEMPLAZO_MMU"))  printf(" El valor ALGORITMO_REEMPLAZO_MMU no existe en la config\n");
    if (!config_has_property(config, "CANTIDAD_ENTRADAS_TLB"))    printf(" El valor CANTIDAD_ENTRADAS_TLB no existe en la config\n");
    if (!config_has_property(config, "ALGORITMO_REEMPLAZO_TLB"))  printf(" El valor ALGORITMO_REEMPLAZO_TLB no existe en la config\n");
    if (!config_has_property(config, "RETARDO_ACIERTO_TLB"))      printf(" El valor RETARDO_ACIERTO_TLB no existe en la config\n");
    if (!config_has_property(config, "RETARDO_FALLO_TLB"))        printf(" El valor RETARDO_FALLO_TLB no existe en la config\n");
    if (!config_has_property(config, "PATH_DUMP_TLB"))            printf(" El valor PATH_DUMP_TLB no existe en la config\n");

    configuracion.IP                       = config_get_string_value(config, "IP");
    configuracion.PUERTO                   = config_get_string_value(config, "PUERTO");
    configuracion.TAMANIO                  = config_get_int_value(config, "TAMANIO");
    configuracion.TAMANIO_PAGINAS          = config_get_int_value(config, "TAMANIO_PAGINA");
    configuracion.TIPO_ASIGNACION          = config_get_string_value(config, "TIPO_ASIGNACION");
    configuracion.MARCOS_POR_CARPINCHO     = config_get_int_value(config, "MARCOS_POR_CARPINCHO");
    configuracion.ALGORITMO_REEMPLAZO_MMU  = config_get_string_value(config, "ALGORITMO_REEMPLAZO_MMU");
    configuracion.CANTIDAD_ENTRADAS_TLB    = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
    configuracion.ALGORITMO_REEMPLAZO_TLB  = config_get_string_value(config, "ALGORITMO_REEMPLAZO_TLB");
    configuracion.RETARDO_ACIERTO_TLB      = config_get_int_value(config, "RETARDO_ACIERTO_TLB");
    configuracion.RETARDO_FALLO_TLB        = config_get_int_value(config, "RETARDO_FALLO_TLB");
    configuracion.PATH_DUMP_TLB            = config_get_string_value(config, "PATH_DUMP_TLB");
}

void iniciar_logger(){
    logger_memoria = log_create("cfg/memoria.log", "Memoria", 0, LOG_LEVEL_INFO);
}

void loggear_mensaje(char* mensaje){
    log_info(logger_memoria, mensaje);
}




