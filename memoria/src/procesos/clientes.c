#include "clientes.h"

#include <utils/utils.h>
#include <sys/socket.h>
#include <pthread.h>

void atender_proceso(void* arg){
    int cliente = *(int*)arg;
    free(arg);
    iniciar_proceso(cliente);
    // ejecutar_proceso(cliente);
}

void iniciar_proceso(int cliente){
    // si no puede ser iniciado: pthread_exit(0)
}