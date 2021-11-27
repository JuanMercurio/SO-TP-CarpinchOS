#include "deadlock.h"
#include "../io_semaforos/io_semaforos.h"


void deteccion_deadlock()
{
    sem_kernel *semaforo;
    t_deadlock *a_enlistar;
    t_list *lista_posible_deadlock = list_create();
    while(!terminar)
    {
        usleep(configuracion.TIEMPO_DEADLOCK);
        log_info(logger, "Comienza la ejecución de la detección de Deadlock");
        //pausar_todo????
        for (int i = 0; i > list_size(lista_sem_kernel); i++)
        {
            //mutex
            sem_wait(&mutex_lista_sem_kernel);
            semaforo = (sem_kernel *)list_get(lista_sem_kernel, i);
            sem_post(&mutex_lista_sem_kernel);
            a_enlistar = buscar_en_otras_listas(semaforo->tomado_por, i, semaforo->id);// ruido
            if (a_enlistar != NULL)
            {
                list_add(lista_posible_deadlock, (void *)a_enlistar);
            }
        }

        log_info(logger, "Finaliza la búsqueda de carpinchos en posible deadlock");

        if (!list_is_empty(lista_posible_deadlock))
        {
            log_info(logger, "Verificando espera circular");
            t_list *lista_con_deadlock = verificar_espera_circular(lista_posible_deadlock);

            while (lista_con_deadlock == NULL)
            {
                finalizar_involucrados(lista_con_deadlock); // falta codear
                                                            //  list_destroy_and_destroy_elements(lista_posible_deadlock, lista_deadlock_destroyer);
                lista_con_deadlock = verificar_espera_circular(lista_posible_deadlock);
            }
        }
        else{
                log_info(logger, "No se encontraron carpinchos en posible deadlock");
        }
    }
}
void lista_deadlock_destroyer(void *arg)
{
    t_deadlock *a_destruir = (t_deadlock *)arg;
    free(a_destruir->retenido);
    free(a_destruir->esperando);
    free(a_destruir);
}

t_deadlock *buscar_en_otras_listas(int pid, int index, char *semaforo_retenido)
{
    //  t_list *lista_peticiones = list_create();
    t_pcb *carpincho_bloqueado;
    int i = 0;
    bool encontrado = false;
    t_deadlock *carpincho_deadlock = malloc(sizeof(t_deadlock));
    while (i < list_size(lista_sem_kernel) && !encontrado)
    {
        if (i == index)
            continue;
        sem_kernel *semaforo = (sem_kernel *)list_get(lista_sem_kernel, i);
        while(!queue_is_empty(semaforo->bloqueados) && !encontrado)
        {
            carpincho_bloqueado = (t_pcb *)queue_pop(semaforo->bloqueados);
            if (pid == carpincho_bloqueado->pid)
            {
                log_info(logger, "El carpincho %d está en posible deadlock - Semáforo retenido: %s - Semáforo por el que se bloqueó: %s", pid, semaforo_retenido, semaforo->id);
                carpincho_deadlock->pid = pid;
                strcpy(carpincho_deadlock->retenido, semaforo_retenido);
                strcpy(carpincho_deadlock->esperando, semaforo->id);
                encontrado = true;
            }
            else
            {
                carpincho_deadlock = NULL;;
            }
        }
        i++;
    }
    return carpincho_deadlock;
}
t_list *verificar_espera_circular(t_list *lista)
{
    t_list *lista_aux = list_create();

    bool cerrado = false;
    while (!list_is_empty(lista))
    {
        while (!list_is_empty(lista) || !cerrado)
        {

            t_deadlock *a_evaluar = list_remove(lista, 0);
            cerrado = encontrar_circulo(a_evaluar, lista, lista_aux);
        }
    }
    if (cerrado)
    {
        log_info(logger, "Espera circular detectada");
        return lista_aux;
    }
    else{
        log_info(logger, "No se detectó espera circular");    
        return NULL;
    }
}
//--------------------------------------------------------------
bool encontrar_circulo(t_deadlock *a_evaluar, t_list *lista, t_list *lista_aux)
{
    int i =0;
    list_add(lista_aux, (void*) a_evaluar);
    int *index = malloc(sizeof(int));
    if (comparar_lista(a_evaluar, lista_aux, index))
    {
        while(i <*index)
        {   
            list_remove_and_destroy_element(lista_aux, i, lista_deadlock_destroyer);
            
        }return true;
    }
    else
    { //

        if (comparar_lista(a_evaluar, lista, index))
        {


            a_evaluar = list_remove(lista, *index);//-------
            encontrar_circulo(a_evaluar, lista, lista_aux);
        }
        return false;// aca se rompio
    }

    free(index);
}

//------------------------------------------------------
bool comparar_lista(t_deadlock *alf, t_list *list, int *index)
{
    t_deadlock *aux;
    int i = 0;
    bool encontro = false;
    if (!list_is_empty(list))
    {
        while (i < list_size(list) && !encontro)
        {
            aux = list_get(list, i);
            if (strcmp(alf->esperando, aux->retenido) == 0)
            {
                *index = i;
                encontro = true;
            }
            else
            {
                i++;
            }
        }
        return encontro;
    }
    else
    {
        return false;
    }
}

void finalizar_involucrados(t_list *lista_deadlock)
{
    int mayor_id = 0;
    t_deadlock *aux;
    char *retenido_a_liberar = string_new();
    char *esperando_a_sacar = string_new();
    sem_kernel *semaforo;
    for (int i=0; i < list_size(lista_deadlock); i++)
    {
        aux = list_get(lista_deadlock, i);
        if (mayor_id < aux->pid)
        {
            mayor_id = aux->pid;
            strcpy(retenido_a_liberar, aux->retenido);
            strcpy(esperando_a_sacar, aux->esperando);
        }
    }
    log_info(logger, "Se finaliza el carpincho PID %d", mayor_id);
    semaforo = buscar_semaforo(esperando_a_sacar);
    sacar_de_cola_bloqueados(semaforo, mayor_id);
    sem_kernel_post(retenido_a_liberar);
}

void sacar_de_cola_bloqueados(sem_kernel *semaforo, int id)
{
    t_queue *cola_auxiliar = queue_create();
    t_pcb *carpincho;
    while (!queue_is_empty(semaforo->bloqueados))
    {
        sem_wait(&semaforo->mutex_cola);
        carpincho = (t_pcb *)queue_pop(semaforo->bloqueados);
        sem_post(&semaforo->mutex_cola);
        if (carpincho->pid == id)
        {
            close(carpincho->fd_cliente);
            close(carpincho->fd_memoria);
            sem_wait(&mutex_cola_finalizados);
            queue_push(cola_finalizados, (void *)carpincho);
            sem_post(&mutex_cola_finalizados);
            sem_post(&cola_finalizados_con_elementos);
        }
        else
        {

            queue_push(cola_auxiliar, (void *)carpincho);
        }
    }
    while (!queue_is_empty(cola_auxiliar))
    {
        carpincho = (t_pcb *)queue_pop(cola_auxiliar);
        sem_wait(&semaforo->mutex_cola);
        queue_push(semaforo->bloqueados, (void *)carpincho);
        sem_post(&semaforo->mutex_cola);
    }
}
