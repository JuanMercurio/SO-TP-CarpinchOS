#include "config.h"

void obtener_config(){
    config = config_create("cfg/memoria.config");
    configuracion.IP = config_get_string_value(config, "IP");
    configuracion.PUERTO = config_get_string_value(config, "PUERTO");
    configuracion.TIPO_ASIGNACION= config_get_string_value(config, "TIPO_ASIGNACION");
    configuracion.ALGORITMO_REEMPLAZO_MMU = config_get_string_value(config, "ALGORITMO_REEMPLAZO_MMU");
    configuracion.ALGORITMO_REEMPLAZO_TLB = config_get_string_value(config, "ALGORITMO_REEMPLAZO_TLB");
    configuracion.CANTIDAD_ENTRADAS_TLB = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
    configuracion.TAMANIO = config_get_int_value(config, "TAMANIO");
    configuracion.RETARDO_ACIERTO_TLB  = config_get_int_value(config, "RETARDO_ACIERTO_TLB");
    configuracion.RETARDO_FALLO_TLB  = config_get_int_value(config, "RETARDO_FALLO_TLB");
}
