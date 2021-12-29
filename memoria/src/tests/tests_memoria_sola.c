#include "tests_memoria_sola.h"

#include <stdlib.h>

void memoria_test_solo(int c_args)
{
    if(c_args != 2)return;

    obtener_config();
    iniciar_logger();
    iniciar_signals();
    iniciar_paginacion();
    iniciar_tlb();

    tab_pags* tabla = malloc(sizeof(tab_pags));
    tabla->pid = 123;

    iniciar_paginas(tabla);
    iniciar_paginas(tabla);
    iniciar_paginas(tabla);
    iniciar_paginas(tabla);
    iniciar_paginas(tabla);

    procesos_imprimir();    

    printf("\nAbortar:\n");
    abort();
}

void procesos_imprimir()
{
    int tamanio = list_size(tablas.lista);

    for(int i=0; i<tamanio; i++)
    { 
        tab_pags* reg =  list_get(tablas.lista, i);       
        printf("PID: %d\n" ,reg->pid);
    }
}
