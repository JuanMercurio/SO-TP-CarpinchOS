#include "operaciones.h"

int  memalloc(int tamanio){

    /* Asegurarnos que entra en memoria:
            1) Buscamos si hay un alloc free en 
               el cual entre (usando First Fit) --> Dividimos el alloc en 2 --> el ocupado y el free 
            2) Si no encuentra entonces creamos un alloc al final, si no hay espacio pido una pagina nueva. || Este renglon ni idea 
            3) Tambien hay que ir a buscar a swamp
    */
   return 1;    //retorna la direccion logica a el inicio del bloque (no del alloc) -- Si no encontro retorna un puntero null
    }

void memfree();

void* memread();

void memwrite();