#ifndef _COMPORTAMIENTO_CLIENTES_
#define _COMPORTAMIENTO_CLIENTES_

/*
    @NAME:  atender_proceso
    @DESC:  hilo que atiene a los procesos que se conectan
            a memoria
 */
void atender_proceso(void* arg);

/*
    @NAME:  ejecutar_proceso
    @DESC:  ejecuta los mensajes que le llegan al hilo de los 
            carpinchos
 */
void ejecutar_proceso(int cliente);

/*
    @NAME:  iniciar_proceso
    @DESC:  inicia el proceso nuevo en memoria
 */
void iniciar_proceso(int cliente);

#endif