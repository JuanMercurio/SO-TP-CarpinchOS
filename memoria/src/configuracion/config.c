#include "config.h"

#include <pthread.h>

t_log* logger_memoria;
pthread_mutex_t mutex_log;

t_config* config;
t_cfg configuracion;

void obtener_config(){

    config = config_create("cfg/memoria.config");

    configuracion.IP                       = config_get_string_value(config, "IP");
    configuracion.PUERTO                   = config_get_string_value(config, "PUERTO");
    configuracion.ALGORITMO_REEMPLAZO_MMU  = config_get_string_value(config, "ALGORITMO_REEMPLAZO_MMU");
    configuracion.TIPO_ASIGNACION          = config_get_string_value(config, "TIPO_ASIGNACION");
    configuracion.ALGORITMO_REEMPLAZO_TLB  = config_get_string_value(config, "ALGORITMO_REEMPLAZO_TLB");
    configuracion.PATH_DUMP_TLB            = config_get_string_value(config, "PATH_DUMP_TLB");
    configuracion.RETARDO_ACIERTO_TLB      = config_get_int_value(config, "RETARDO_ACIERTO_TLB");
    configuracion.RETARDO_FALLO_TLB        = config_get_int_value(config, "RETARDO_FALLO_TLB");
    configuracion.TAMANIO                  = config_get_int_value(config, "TAMANIO");
    configuracion.TAMANIO_PAGINAS          = config_get_int_value(config, "TAMANIO_PAGINAS");
    configuracion.MARCOS_POR_CARPINCHO     = config_get_int_value(config, "MARCOS_POR_CARPINCHO");
    configuracion.CANTIDAD_ENTRADAS_TLB    = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
}

void iniciar_logger(){
    logger_memoria = log_create("cfg/memoria.log", "Memoria", 0, LOG_LEVEL_INFO);
    pthread_mutex_init(&mutex_log, 0);
}

void loggear_mensaje(char* mensaje){
    pthread_mutex_lock(&mutex_log);
    log_info(logger_memoria, mensaje);
    pthread_mutex_unlock(&mutex_log);
}




