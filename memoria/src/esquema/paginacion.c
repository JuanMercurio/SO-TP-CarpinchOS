#include "paginacion.h"
#include "../configuracion/config.h"
#include "algoritmos.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

void iniciar_paginacion(){

    ids_memoria = 0;
    init_ram();
    init_bitmap_frames();
    set_asignacion();
}

void init_ram(){
    ram.memoria = malloc(configuracion.TAMANIO);
    pthread_mutex_init(&ram.mutex, NULL);
}

void init_bitmap_frames(){
    int n_frames = configuracion.TAMANIO / configuracion.TAMANIO_PAGINAS;
    marcos = calloc(n_frames, sizeof(int));
}

void set_asignacion(){
    if(strcmp(configuracion.TIPO_ASIGNACION, "FIJA") == 0)
    {
        lru = &lru_fijo;
    }
    else
    {
        lru = &lru_dinamico;
    }
}

void add_new_page_table(tab_pags* tabla){
    pthread_mutex_lock(&tablas.mutex);
    list_add(tablas.lista, tabla);
    pthread_mutex_unlock(&tablas.mutex);
}

tab_pags* buscar_page_table(int pid){ 
    for(int i=0; i<list_size(tablas.lista); i++){
        tab_pags* tabla = list_get(tablas.lista, i);
        if(tabla->pid == pid) return tabla;
    }
    return NULL;
}

int get_offset(uint32_t dl, int multiplier){
    double temp = dl / (float)multiplier;
    int paginas = temp;
    float offset = (temp - paginas)* multiplier;
    return (int)offset;
}

dir_t traducir_dir_log(uint32_t dir_log){

    // Estas variables podrian ser globales y calcularlas con la config
    int max_offset = configuracion.TAMANIO_PAGINAS;
    // Cantidad de bytes que necesita para representar el offset
    int bytes_offset = log(max_offset) / log(2);

    int multiplier = pow(10, bytes_offset);
    
    dir_t dir;
    dir.PAGINA = binario_a_decimal(dir_log / multiplier);
    dir.offset = binario_a_decimal(get_offset(dir_log, multiplier));
    return dir;
}

uint32_t crear_dl(dir_t dl){

    // Estas variables podrian ser globales y calcularlas con la config
    int max_offset = configuracion.TAMANIO_PAGINAS;
    // Cantidad de bytes que necesita para representar el offset
    int bytes_offset = log(max_offset) / log(2);

    int multiplier = pow(10, bytes_offset);

    int pagina_b = decimal_a_binario(dl.PAGINA);
    int offset_b = decimal_a_binario(dl.offset);
    
    uint32_t dir = (pagina_b * multiplier) + offset_b;
    return dir;
}

dir_t convertir_a_df(t_list* tabla, dir_t dl){
    dir_t df;
    pag_t* reg = list_get(tabla, dl.PAGINA);
    df.offset = dl.offset;
    df.MARCO = reg->marco;
    return df;
}

int offset_memoria(dir_t df){
    return df.MARCO * configuracion.TAMANIO_PAGINAS + df.offset;
}

uint32_t decimal_a_binario(int decimal) {
    int binario=0, resto=0, pos=1;

	while (decimal) {
		resto = decimal % 2;
		decimal /= 2;
		binario += resto * pos;
		pos *= 10;
	}

	return binario;
}

int binario_a_decimal(uint32_t binario) {
    int decimal = 0, i = 0, resto;
    while (binario != 0) {
    	resto = binario % 10;
        binario /= 10;
        decimal += resto * pow(2, i);
        ++i;
    }
    return decimal;
}
