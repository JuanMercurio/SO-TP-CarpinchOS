#include "tests_memoria_sola.h"

void memoria_test_solo(int c_args)
{
    if(c_args != 2)return;

    obtener_config();
    iniciar_logger();
    iniciar_signals();
    iniciar_paginacion();
    iniciar_tlb();

    iniciar_paginas(123, 1);
    iniciar_paginas(123, 10);
    iniciar_paginas(123, 1010);
    iniciar_paginas(123, 1337);
    iniciar_paginas(123, 1986);

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