#include "paginacion.h"
#include "tlb.h"
#include "../procesos/operaciones.h"

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
t_list *marcos;
int ids_memoria = 1;

/* --------------- INICIACION ---------------------- */

void iniciar_paginacion()
{

    init_ram();
    init_bitmap_frames();
    init_estructuras();
    set_algoritmos();
}

void init_ram()
{
    ram.memoria = malloc(configuracion.TAMANIO);
    pthread_mutex_init(&ram.mutex, NULL);
}

void init_estructuras()
{
    tablas.lista = list_create();
    pthread_mutex_init(&tablas.mutex, NULL);
}

void init_bitmap_frames()
{
    marcos = list_create();

    int n_frames = configuracion.TAMANIO / configuracion.TAMANIO_PAGINAS;

    for (int i = 0; i < n_frames; i++)
    {
        int *marco = malloc(sizeof(int));
        *marco = VACIO;
        list_add(marcos, marco);
    }
}

/* --------------- Paginas ---------------------- */

// insertar pagina asignacion fija
int insertar_pagina_af(void *contenido, tab_pags *tabla)
{

    if (marcos_maximos_asignados(tabla))
    {
        return -1;
    }

    else
    {
        int marco = marco_libre();
        if (marco != -1)
            return marco;
    }

    return -1;
}

// insertar pagina asignacion dinamica
int insertar_pagina_ad(void *contenido)
{

    int marco = marco_libre();
    if (marco != -1)
        return marco;

    return -1;
}

bool marcos_maximos_asignados(tab_pags *tabla)
{
    return configuracion.MARCOS_POR_CARPINCHO < paginas_presentes(tabla);
}

int paginas_presentes(tab_pags *t)
{
    int presente = 0;
    int tamanio = list_size(t->tabla_pag);
    for (int i = 0; i < tamanio; i++)
    {
        pag_t *p = list_get(t->tabla_pag, i);
        if (p->presente == 1)
            presente++;
    }

    return presente;
}

int nro_marco(int pagina, tab_pags *tabla)
{
    if (!pagina_valida(tabla, pagina))
        return PAGINA_INVALIDA;

    int marco;

    // Busco en TLB
    marco = buscar_en_tlb(tabla, pagina);
    if (marco != TLB_MISS) {
        printf("Entontre en la tlb\n");
        return marco; 
    }

    // Busco en tabla de paginas
    marco = buscar_en_tabPags(tabla, pagina);
    if (marco != MEMP_MISS) {
        printf("Encontre en tabla de paginas\n");
        return marco; 
    }

    marco = buscar_en_swap(tabla, pagina);
    if (marco != -1) {
        printf("Encontre en swap");
        return marco;
    }

    return -1;
}

int buscar_en_swap(tab_pags *tabla, int pagina)
{
    printf(" SWAP - PID: %d - PAG: %d \n", tabla->pid, pagina);
    enviar_int(swap, OBTENER_PAGINA);
    enviar_int(swap, tabla->pid);
    enviar_int(swap, pagina);

    int op = recibir_int(swap);

    if (op == -1) {
        printf("SWAP: no se encontro la pagina en swap\n");
        return -1;
    }

    void* contenido = recibir_buffer(configuracion.TAMANIO_PAGINAS, swap);
    printf("RECIBIDO: PID: %d, PAG: %d \n", tabla->pid, pagina);
    printf("Recibido = Contenido isfree %d \n", ((HeapMetadata*)contenido)->isFree);
    printf("Recibido = Contenido prevalloc %d \n", ((HeapMetadata*)contenido)->prevAlloc);
    printf("Recibido = Contenido nextalloc %d \n", ((HeapMetadata*)contenido)->nextAlloc);
    int receptor = recibir_int(swap);
    if (receptor == -1) {
        printf("No existe la pagina en swap\n");
        abort();
    }

    t_victima victima = algoritmo_mmu(tabla->pid, tabla);
    reemplazar_pagina(victima, contenido, pagina, tabla);

    return victima.marco;
}

void reemplazar_pagina(t_victima victima, void *buffer, int pagina, tab_pags *tabla)
{
    if(victima.tlb == 1) tlb_limpiar_registro(victima.pid, victima.pagina);
    // y si buffer es NULL?

    if (victima.modificado == 1) {
       enviar_pagina_a_swap(victima.pid, victima.pagina, victima.marco);
       int pido = recibir_int(swap);
    }

    tlb_insert_page(tabla->pid, pagina, victima.marco, READ);
    insertar_pagina(buffer, victima.marco);
    actualizar_nueva_pagina(pagina, victima.marco, tabla);
    printf("Reemplace la pagina %d del pid %d \n", victima.pagina, victima.pid);
}

pag_t* obtener_pagina(int pid, int pagina)
{
    tab_pags* tabla = buscar_page_table(pid);
    return list_get(tabla->tabla_pag, pagina);
}

void tlb_limpiar_registro(int pid, int pagina) 
{
    int tamanio = list_size(tlb);
    for (int i=0; i < tamanio; i++) {
        tlb_t* reg = list_get(tlb, i);
        if ( reg->pid == pid && reg->pagina == pagina) {
            reg->pid = -1;
            return;
        }
    }
}

void actualizar_nueva_pagina(int pagina, int marco, tab_pags *tabla)
{

    pag_t *reg = list_get(tabla->tabla_pag, pagina);
    reg->presente = 1;
    if (list_size(tlb) != 0) reg->tlb = 1;
    reg->marco = marco;
    reg->algoritmo = alg_comportamiento();

}
void actualizar_victima_de_tlb(tlb_t* reg)
{
    tab_pags* tabla = buscar_page_table(reg->pid);
    pag_t* pagina = list_get(tabla->tabla_pag, reg->pagina);
    
    pagina->algoritmo = reg->alg;
    pagina->marco = reg->marco;
    pagina->modificado = reg->modificado;
    pagina->tlb = 0;
}

void *recibir_marco(int cliente)
{
    void *buffer = malloc(0);
    return buffer;
}

void insertar_pagina(void *pagina, int marco)
{
    if (pagina == NULL) return;
    memcpy(ram.memoria + marco * configuracion.TAMANIO_PAGINAS, pagina, configuracion.TAMANIO_PAGINAS);
}

void *recibir_contenido(int swap)
{
    void *buffer = malloc(configuracion.TAMANIO_PAGINAS);
    buffer = recibir_marco(swap);
    return buffer;
}

void *serializar_pedido_pagina(int pid, int pagina)
{
    int size = sizeof(int) * 2;
    void *buffer = malloc(size);
    memcpy(buffer, &pid, sizeof(int));
    memcpy(buffer + sizeof(int), &pagina, sizeof(int));
    return buffer;
}

bool pagina_valida(tab_pags *tabla, int pagina)
{
    int n_paginas = list_size(tabla->tabla_pag);
    return n_paginas > pagina;
}

int buscar_en_tabPags(tab_pags *tabla, int pagina)
{

    pag_t *reg = list_get(tabla->tabla_pag, pagina);

    if (reg->presente == 1)
    {
        actualizar_tlb(tabla->pid, reg->marco, pagina);
        return reg->marco;
    }

    return MEMP_MISS;
}

int crear_pagina(t_list *paginas)
{
    pag_t *pagina = malloc(sizeof(pag_t));
    pagina->presente = 0;
    pagina->marco = NOT_ASIGNED;
    pagina->modificado = 0;
    pagina->algoritmo = NOT_ASIGNED;
    pagina->tlb = 0;
    return list_add(paginas, pagina);
}

int marco_libre()
{
    int n_frames = configuracion.TAMANIO / configuracion.TAMANIO_PAGINAS;
    for (int i = 0; i < n_frames; i++)
    {
        int *marco = list_get(marcos, i);
        if (*marco == VACIO)
            {
                *marco = NO_VACIO;
                return i;
            }
    }
    return NOT_ASIGNED;
}

void pagina_iniciar(tab_pags *tabla)
{
    //verificar en swap
    int pagina = crear_pagina(tabla->tabla_pag);

    int marco = marco_libre();

    if (marco == -1)
    {
        t_victima victima = algoritmo_mmu(tabla->pid, tabla);
        reemplazar_pagina(victima, NULL, pagina, tabla);
    }

    else
    {
        // int* victima = list_get(marcos, marco);
        // *victima = 1;
        memoria_asignar_pagina_vacia(tabla, pagina, marco);
    }

    
}

void memoria_asignar_pagina_vacia(tab_pags* tabla, int pagina, int marco)
{
    pag_t* p = list_get(tabla->tabla_pag, pagina);

    page_use(tabla->pid, marco, p, pagina, READ);
}

void add_new_page_table(tab_pags *tabla)
{
    list_add(tablas.lista, tabla);
}

tab_pags *buscar_page_table(int pid)
{
    for (int i = 0; i < list_size(tablas.lista); i++)
    {
        tab_pags *tabla = list_get(tablas.lista, i);
        if (tabla->pid == pid)
            return tabla;
    }
    return NULL;
}

/* -------------- DIRECCIONAMIENTO ---------------- */

uint32_t crear_dl(dir_t dl)
{

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

dir_t traducir_dir_log(uint32_t dir_log)
{

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

int get_offset(uint32_t dl, int multiplier)
{
    double temp = dl / (float)multiplier;
    int paginas = temp;
    float offset = (temp - paginas) * multiplier;
    return (int)offset;
}

dir_t convertir_a_df(t_list *tabla, dir_t dl)
{
    dir_t df;
    pag_t *reg = list_get(tabla, dl.PAGINA);
    df.offset = dl.offset;
    df.MARCO = reg->marco;
    return df;
}

int offset_memoria(dir_t df)
{
    return df.MARCO * configuracion.TAMANIO_PAGINAS + df.offset;
}

uint32_t decimal_a_binario(int decimal)
{
    int binario = 0, resto = 0, pos = 1;

    while (decimal)
    {
        resto = decimal % 2;
        decimal /= 2;
        binario += resto * pos;
        pos *= 10;
    }

    return binario;
}

int binario_a_decimal(uint32_t binario)
{
    int decimal = 0, i = 0, resto;
    while (binario != 0)
    {
        resto = binario % 10;
        binario /= 10;
        decimal += resto * pow(2, i);
        ++i;
    }
    return decimal;
}

dir_t decimal_a_dl(int dir_log)
{
    dir_t dl;
    dl.offset = dir_log % configuracion.TAMANIO_PAGINAS;
    dl.PAGINA = dir_log / configuracion.TAMANIO_PAGINAS;
    return dl;
}

int dl_a_decimal(dir_t dl)
{
    int decimal = 0;
    decimal += dl.PAGINA * configuracion.TAMANIO_PAGINAS;
    decimal += dl.offset;
    return decimal;
}

bool pagina_solicitud_swap(int pid)
{
    t_paquete *p = crear_paquete(SOLICITUD_PAGINA);
    agregar_a_paquete(p, &pid, sizeof(int));
    enviar_paquete(p, swap);
    eliminar_paquete(p);

    return recibir_int(swap);
}

void *pagina_obtener_swap(int pid, int pagina)
{
    t_paquete *p = crear_paquete(OBTENER_PAGINA);
    agregar_a_paquete(p, &pid, sizeof(int));
    agregar_a_paquete(p, &pagina, sizeof(p));
    enviar_paquete(p, swap);
    eliminar_paquete(p);

    void *buffer = recibir_buffer(configuracion.TAMANIO_PAGINAS, swap);
    return buffer;
}

void enviar_pagina_a_swap(int pid, int pagina, int marco)
{
    enviar_int(swap, ESCRIBIR_PAGINA);
    enviar_int(swap, pid);
    enviar_int(swap, pagina);
    enviar_int(swap, configuracion.TAMANIO_PAGINAS);

    void *contenido = malloc(configuracion.TAMANIO_PAGINAS);

    memcpy(contenido, ram.memoria + marco * configuracion.TAMANIO_PAGINAS, configuracion.TAMANIO_PAGINAS);
    enviar_buffer(swap, contenido, configuracion.TAMANIO_PAGINAS);
    printf("SWAP Le envie a- PID: %d - PAG: %d  enviado \n", pid, pagina);
    printf("Contenido isfree %d \n", ((HeapMetadata*)contenido)->isFree);
    printf("Contenido prevalloc %d \n", ((HeapMetadata*)contenido)->prevAlloc);
    printf("Contenido nextalloc %d \n", ((HeapMetadata*)contenido)->nextAlloc);

    // Este codigo comentado es para mandar las cosas de una por ahora no lo usamoso
    // t_paquete *paquete = crear_paquete(ESCRIBIR_PAGINA);
    // void *contenido = malloc(configuracion.TAMANIO_PAGINAS);
    // memcpy(contenido, ram.memoria + marco * configuracion.TAMANIO_PAGINAS, configuracion.TAMANIO_PAGINAS);

    // agregar_a_paquete(paquete, &pagina, sizeof(int));
    // agregar_a_paquete(paquete, contenido, configuracion.TAMANIO_PAGINAS);

    // enviar_paquete(paquete, swap);

    free(contenido);
}