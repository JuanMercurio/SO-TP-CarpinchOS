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

int server;
bool asignacionFija;
t_list* carpinchosEnArchivo;

typedef struct {
    int pid;
    int numeroArchivo;
    int cantidadPaginas;
    int orden;
} Carpincho_Swamp;
/*
    @NAME: terminar_programa
    @DESC: Se encarga de liberar todas las estructuras y de hacer lo necesario para
           que el programa termine sin memory leaks y sin errores
 */

int elegirMejorArchivo();
void crearCarpincho (int pid, int pag, char* contenidoPagina, char letra);
int cantidadCaracteresFile (char* path);
void crearArchivos();
int remplazoPaginaFija(int pid, int pagina, char*contenido);
int buscarArchivoDeCarpincho (int pidd);
int buscarOrdenParaAgregar(int num_archivo);
Carpincho_Swamp* buscarCarpincho(int pidd);
bool quedaEspacioEnArchivoFija();
char* buscarPagina(int pid, int pag);
void terminar_programa();
void iniciar_swamp();
void atender_clientes();


/*
    @NAME:  resolver_estado
    @DESC:  envia a memoria si el proceso puede empezar. 
    @RETN:  Retora el id del proceso que inicia. Si no puede iniciar retora -1
 */
int resolver_estado(int , int );

#endif
