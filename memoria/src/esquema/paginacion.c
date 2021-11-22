#include "paginacion.h"
#include "tlb.h"

#include <mensajes/mensajes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <utils/utils.h>

#define MEMP_MISS -1
#define PAGINA_INVALIDA -2

memoria_t ram;
tablas_t tablas;
t_list* marcos;
int ids_memoria = 1;

/* --------------- INICIACION ---------------------- */

void iniciar_paginacion(){
	
    init_ram();
    init_bitmap_frames();
    init_estructuras();
    set_algoritmos();
}

void init_ram(){
    ram.memoria = malloc(configuracion.TAMANIO);
    pthread_mutex_init(&ram.mutex, NULL);
}

void init_estructuras(){
    tablas.lista = list_create();
    pthread_mutex_init(&tablas.mutex, NULL);
}

void init_bitmap_frames(){
    marcos = list_create();

    int n_frames = configuracion.TAMANIO / configuracion.TAMANIO_PAGINAS;

    for(int i=0; i<n_frames; i++){
        int* marco = malloc(sizeof(int));
        *marco = VACIO;
        list_add(marcos, marco);
    }
}

/* --------------- Paginas ---------------------- */ 
// insertar pagina asignacion fija
int insertar_pagina_af(void* contenido, tab_pags* tabla){

    if(marcos_maximos_asignados(tabla))
    { 
        // reemplazar_pagina();
    }

    else
    {
        int marco = marco_libre();
        if(marco != -1) return marco;
    }
    
    return -1;
}

// insertar pagina asignacion dinamica
int insertar_pagina_ad(void* contenido){

    int marco = marco_libre();
    if(marco != -1) return marco;
}

bool marcos_maximos_asignados(tab_pags* tabla)
{
    return configuracion.MARCOS_POR_CARPINCHO < list_size(tabla->tabla_pag);
}

int nro_marco(int pagina, tab_pags* tabla){

    if(!pagina_valida(tabla, pagina)) return PAGINA_INVALIDA;

    int marco;

    // Busco en TLB
    marco = buscar_en_tlb(tabla, pagina);
    if(marco != TLB_MISS) return marco;

    // Busco en tabla de paginas
    marco = buscar_en_tabPags(tabla, pagina);
    if(marco != MEMP_MISS) return marco;

    marco = buscar_en_swap(tabla, pagina);
    if(marco != -1) return marco;

    return -1;
}

int buscar_en_swap(tab_pags* tabla, int pagina){

    void* buffer = serializar_pedido_pagina(tabla->pid, pagina);
    t_paquete* paquete = crear_paquete(SOLICITUD_PAGINA);
    agregar_a_paquete(paquete, buffer, sizeof(int)*2);

    int swap = crear_conexion("127.0.0.1", "5003");
    enviar_paquete(paquete, swap);
    
    int op = recibir_int(swap);

    if (op == -1) { 
        close(swap);
        return -1; 
    }
    
    void* contenido = recibir_contenido(swap);
    close(swap);

    t_victima victima = algoritmo_mmu(tabla->pid, tabla);
    reemplazar_pagina(victima, contenido, pagina, tabla);

    return victima.marco;
}

void reemplazar_pagina(t_victima victima, void* buffer, int pagina, tab_pags* tabla){

    if(victima.modificado == 1) enviar_pagina_a_swap(victima.pid, victima.pagina, victima.marco);
    insertar_pagina(buffer, victima.marco);
    actualizar_nueva_pagina(pagina, victima.marco, tabla);
}


void actualizar_nueva_pagina(int pagina, int marco, tab_pags* tabla)
{
    pthread_mutex_lock(&tablas.mutex);

    pag_t* reg = list_get(tabla->tabla_pag, pagina);
    reg->presente = 1;
    reg->marco = marco;

    pthread_mutex_unlock(&tablas.mutex);
}

void* recibir_marco(int cliente){
    void* buffer = malloc(0);
    return buffer;
}

void insertar_pagina(void* pagina, int marco){

    pthread_mutex_lock(&ram.mutex);
    memcpy(ram.memoria + marco*configuracion.TAMANIO_PAGINAS, pagina, configuracion.TAMANIO_PAGINAS);
    pthread_mutex_unlock(&ram.mutex);
}

void* recibir_contenido(int swap){
    void* buffer = malloc(configuracion.TAMANIO_PAGINAS);
    buffer = recibir_marco(swap);
    return buffer;
}

void* serializar_pedido_pagina( int pid, int pagina)
{
    int size = sizeof(int)*2;
    void* buffer = malloc(size);
    memcpy(buffer, &pid, sizeof(int));
    memcpy(buffer + sizeof(int), &pagina, sizeof(int));
    return buffer;
}

bool pagina_valida(tab_pags* tabla, int pagina){

    pthread_mutex_lock(&tablas.mutex);
    int n_paginas = list_size(tabla->tabla_pag);
    pthread_mutex_unlock(&tablas.mutex); 

    return n_paginas > pagina;
}
 
int buscar_en_tabPags(tab_pags* tabla, int pagina){

    pthread_mutex_lock(&tablas.mutex);
    pag_t* reg = list_get(tabla->tabla_pag, pagina);
    pthread_mutex_unlock(&tablas.mutex);
        
    if(reg->presente == 1)
    {
        actualizar_tlb(tabla->pid, reg->marco, pagina);
        return reg->marco; 
    }

    return MEMP_MISS;
}

int crear_pagina(t_list *paginas){
    pag_t *pagina = malloc(sizeof(pag_t));
    pagina->presente = 0;
    pagina->marco = NOT_ASIGNED;
    pagina->modificado = 0;
    pagina->algoritmo = NOT_ASIGNED;
    return list_add(paginas, pagina);
}

int marco_libre(){
    int n_frames = configuracion.TAMANIO / configuracion.TAMANIO_PAGINAS;
    for(int i=0; i<n_frames; i++) {
        int* marco = list_get(marcos, i);
        if(*marco==VACIO) return i;
    }
    return NOT_ASIGNED;
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

/* -------------- DIRECCIONAMIENTO ---------------- */

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

int get_offset(uint32_t dl, int multiplier){
    double temp = dl / (float)multiplier;
    int paginas = temp;
    float offset = (temp - paginas)* multiplier;
    return (int)offset;
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

bool pagina_solicitud_swap(int pid){
    t_paquete* p = crear_paquete(SOLICITUD_PAGINA);
    agregar_a_paquete(p, &pid, sizeof(int));
    enviar_paquete(p, swap);
    eliminar_paquete(p);
    
    return recibir_int(swap);
}

void* pagina_obtener_swap(int pid, int pagina)
{
    t_paquete* p = crear_paquete(OBTENER_PAGINA);
    agregar_a_paquete(p, &pid, sizeof(int));
    agregar_a_paquete(p, &pagina, sizeof(p));
    enviar_paquete(p, swap);
    eliminar_paquete(p);

    void* buffer = recibir_buffer(configuracion.TAMANIO_PAGINAS, swap);
    return buffer;
    
}


void enviar_pagina_a_swap(int pid, int pagina, int marco){
    int swap = crear_conexion("127.0.0.1", "5003");
    
    t_paquete* paquete = crear_paquete(ESCRIBIR_PAGINA);
    void* contenido = malloc(configuracion.TAMANIO_PAGINAS);
    memcpy(contenido, ram.memoria + marco*configuracion.TAMANIO_PAGINAS, configuracion.TAMANIO_PAGINAS);

    agregar_a_paquete(paquete, &pagina, sizeof(int));
    agregar_a_paquete(paquete, contenido, configuracion.TAMANIO_PAGINAS);

    enviar_paquete(paquete, swap);

    free(contenido);
}