#include "config_pag.h"

#include <string.h>

pag_t* (*lru)(int, tab_pags*);

void set_asignacion(){
    if(strcmp(configuracion.TIPO_ASIGNACION, "FIJA") == 0)
    {
        lru = lru_fijo; 
    }
    else
    {
        lru = lru_dinamico;
    }
}
