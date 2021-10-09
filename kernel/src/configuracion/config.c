#include "config.h"

void obtener_config(){
    config = config_create("cfg/kernel.config");
    configuracion.IP = config_get_string_value(config, "IP");
    configuracion.PUERTO= config_get_string_value(config, "PUERTO");
    configuracion.IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    configuracion.ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    configuracion.DISPOSITIVOS_IO = config_get_string_value(config, "DISPOSITIVOS_IO");
    configuracion.DURACIONES_IO = config_get_string_value(config, "DURACIONES_IO");
    configuracion.PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
    configuracion.RETARDO_CPU = config_get_int_value(config, "RETARDO_CPU");
    configuracion.GRADO_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    configuracion.GRADO_MULTIPROCESAMIENTO = config_get_int_value(config, "GRADO_MULTIPROCESAMIENTO");
}