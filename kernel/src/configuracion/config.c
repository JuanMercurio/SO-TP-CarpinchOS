#include "config.h"

void obtener_config(){
    config = config_create("cfg/kernel.config");
    configuracion.IP = config_get_string_value(config, "IP");
    configuracion.PUERTO= config_get_string_value(config, "PUERTO");
    configuracion.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    configuracion.ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    configuracion.DISPOSITIVOS_IO = config_get_array_value(config, "DISPOSITIVOS_IO");
    configuracion.DURACIONES_IO = config_get_array_value(config, "DURACIONES_IO");
    configuracion.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");

    configuracion.RETARDO_CPU =  config_get_int_value(config, "RETARDO_CPU");
    configuracion.GRADO_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    configuracion.GRADO_MULTIPROCESAMIENTO = config_get_int_value(config, "GRADO_MULTIPROCESAMIENTO");

    configuracion.ALPHA = config_get_double_value(config, "ALPHA");
    configuracion.ESTIMACION_INICIAL = config_get_double_value(config, "ESTIMACION_INICIAL");

    configuracion.TIEMPO_DEADLOCK = config_get_int_value(config, "TIEMPO_DEADLOCK");

    log_info(logger, "Configuración obtenida");

}

void iniciar_logger(){
    logger = log_create("cfg/kernel.log", "Kernel", 0, LOG_LEVEL_INFO);
    log_info(logger,"Log creado");
}