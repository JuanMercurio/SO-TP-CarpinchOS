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
#include <stdint.h>

typedef struct HeapMetadata{
    uint32_t prevAlloc;
    uint32_t nextAlloc;
    uint8_t isFree;
}__attribute__((packed)) HeapMetadata;

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
    int marco;
} Marcos_x_pagina;
typedef struct {
    int base;
    int numero_archivo;
} un_marco_libre;
typedef struct Pedido {
    char* nombre_pedido;
    int pid;
    int pagina;
    char* contenido_pagina;
    int oper;
} Pedido;
typedef struct{
    int cant;
}Cantidad_arch_car;

//VARIABLES GLOBALES
int server;
bool asignacionFija;
bool termino;
bool terminar = false;
t_list* lista_carpinchos;
t_list* lista_marcos;
t_list* marcos_libres_fija;
t_list* marcos_libres;
t_list* lista_pedidos;
t_list* cantidad_carpinchos_por_archivo;
sem_t* mutex_lista_pedidos;
sem_t*  agrego_lista_pedidos;
int cant_ped;
extern t_log* logger;
char *LOG_PATH = "./swamp.log";
char *PROGRAM_NAME = "SWAMP";

/*
    @NAME: terminar_programa
    @DESC: Se encarga de liberar todas las estructuras y de hacer lo necesario para
           que el programa termine sin memory leaks y sin errores
 */

void pruebas();
void destroy_and_free (int);
void mostrarSemaforosYListaPedidos(char* mensaje);
void memoria_operacion(int cliente);
void borrar_Carpincho(Carpincho_Swamp* car);
void borrar_paginas_Ocupadas( Marcos_x_pagina* mar_pag);
void borrar_marcos_libres (un_marco_libre* mar_lib);
void mostrarPaginasCarpincho(t_list* paginas);
Marcos_x_pagina* tieneMarcoPaginas ( t_list* paginas, int pagina);
int solicitud_muchas_paginas(int pid, int cantidadPaginas, t_list* paginas);
//GENERAL
void crearArchivos();
int elegirMejorArchivo();
Carpincho_Swamp* buscarCarpincho(int pidd);
int cantidadCaracteresFile (char* path);
bool quedaEspacioEnArchivo();
int cantidadBarra0File (char* path);
bool quedaEspacioEnArchivoDOS();
int elegirMejorArchivoDOS();
int buscarMarcoLibre(int num_archivo);

//ASIGNACION DINAMICA
int agregarPaginaDinamica(int pid, int pagina, char* contenido);
int CrearCarpincho(int pidd);
char* buscarPaginaDinamico(int pid, int pagina);
void marcosLibes();
bool quedaPaginasEnArchivo(int pid);
int solicitudPagina(int pid, int pagina);
int borrarPagina(int pid, int pagina);
int borrarCarpincho(int pid);

//ASIGNACION FIJA

void crearCarpinchoFija (int pid, int pag, char* contenidoPagina, char letra);
int agregarPaginaFija(int pid, int pagina, char* contenido);
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
Marcos_x_pagina* buscarMarcoXPagina( Carpincho_Swamp* car, int pagina);
int BorrarPaginaFija(int pid, int pagina);
bool puedeAgregarPagina( int pid);
int solicitarPaginaFija(int pid, int pagina);
int crearCarpinchoFijaDOS(int pidd);
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
void agregarPedidosMemoria (int cliente);

void memoria_tests(int argc, char* argv);

#endif
