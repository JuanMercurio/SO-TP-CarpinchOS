#ifndef _MAIN_SWAMP_
#define _MAIN_SWAMP_

#include "tests/tests.h"
#include "configuracion/config.h"
#include <utils/utils.h> 
#include <conexiones/conexiones.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>


typedef struct {
    int pid;
    int numeroArchivo;
    int cantidadPaginas;
    int orden;
    int base;
    t_list* paginas;
} Carpincho_Swamp;
typedef struct {
    int pagina;
    int base;
} Marcos_x_pagina;
typedef struct {
    int base;
    int numero_archivo;
} un_marco_libre;
typedef struct {
    char* nombre_pedido;
    int pid;
    int pagina;
    char* contenido_pagina;
} Pedido;

//VARIABLES GLOBALES
int server;
bool asignacionFija;
t_list* lista_carpinchos;
t_list* lista_marcos;
t_list* marcos_libres_fija;
t_list* marcos_libres;
t_list* lista_pedidos;
sem_t* mutex_lista_pedidos;
sem_t*  agrego_lista_pedidos;

/*
    @NAME: terminar_programa
    @DESC: Se encarga de liberar todas las estructuras y de hacer lo necesario para
           que el programa termine sin memory leaks y sin errores
 */
//GENERAL
void crearArchivos();
int elegirMejorArchivo();
Carpincho_Swamp* buscarCarpincho(int pidd);
int cantidadCaracteresFile (char* path);
bool quedaEspacioEnArchivo();
int cantidadBarra0File (char* path);
bool quedaEspacioEnArchivoDOS();

//ASIGNACION DINAMICA
int agregarPaginaDinamica(int pid, int pagina, char* contenido);
int CrearCarpincho(int pidd);
char* buscarPaginaDinamico(int pid, int pagina);
void marcosLibes();

//ASIGNACION FIJA

void crearCarpinchoFija (int pid, int pag, char* contenidoPagina, char letra);
int remplazoPaginaFija(int pid, int pagina, char*contenido);
int buscarArchivoDeCarpincho (int pidd);
int buscarOrdenParaAgregar(int num_archivo);
char* buscarPaginaFija(int pid, int pag);
int borrarCarpinchoFija(int pid);
void cambiarOrdenPorBorado( Carpincho_Swamp* car1);
int elegirBaseCarpincho(int num_archivo);
void mostrarCarpinchos();
void marcosLIbresFija();
void sacarCarpinchoDeLista( int pid);
//CONEXIONES
void terminar_programa();
void iniciar_swamp();
void atender_clientes();
/*
    @NAME:  resolver_estado
    @DESC:  envia a memoria si el proceso puede empezar. 
    @RETN:  Retora el id del proceso que inicia. Si no puede iniciar retora -1
 */
int resolver_estado(int , int );
void agregarPedidosMemoria ();

void memoria_tests(int argc, char* argv);

#endif
