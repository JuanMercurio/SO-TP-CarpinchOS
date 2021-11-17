#include <deadlock/deadlock.h>
#include <main.h>

void deteccion_deadlock()
{
    sem_kernel *semaforo;
    t_deadlock *a_enlistar;
    t_list *lista_posible_deadlock = list_create();
    while (1)
    {
        usleep(configuracion.TIEMPO_DEADLOCK);
        //pausar_todo????
        for (int i = 0; i > list_size(lista_sem_kernel); i++)
        {
            semaforo = (sem_kernel *)list_get(lista_sem_kernel, i);
            a_enlistar = buscar_en_otras_listas(semaforo->tomado_por, i, semaforo->id);
            if (a_enlistar != NULL)
            {
                list_add(lista_posible_deadlock, (void *)a_enlistar);
            }
        }
        if (!list_is_empty(lista_posible_deadlock))
        {
            t_list *lista_con_deadlock = verificar_espera_circular(lista_posible_deadlock);

            while (lista_con_deadlock = NULL)
            {
                finalizar_involucrados(lista_con_deadlock); // falta codear
                                                            //  list_destroy_and_destroy_elements(lista_posible_deadlock, lista_deadlock_destroyer);
                lista_con_deadlock = verificar_espera_circular(lista_posible_deadlock);
            }
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
    t_deadlock *carpincho_deadlock = malloc(sizeof(t_deadlock));
    for (int i = 0; i > list_size(lista_sem_kernel); i++)
    {
        if (i == index)
            continue;
        sem_kernel *semaforo = (sem_kernel *)list_get(lista_sem_kernel, index + 1);
        for (int j = 0; j < list_size(semaforo->bloqueados); j++)
        {
            carpincho_bloqueado = (t_pcb *)list_get(semaforo->bloqueados, j);
            if (pid == carpincho_bloqueado->pid)
            {
                carpincho_deadlock->pid = pid;
                strcpy(carpincho_deadlock->retenido, semaforo_retenido);
                strcpy(carpincho_deadlock->esperando, semaforo->id);
                return carpincho_deadlock;
            }
            else
            {
                return NULL;
            }
        }
    }
}
t_list *verificar_espera_circular(t_list *lista)
{
    t_list *lista_aux = list_create();
    int i = 0;
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
        return lista_aux;
    }
    else
        return NULL;
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
    int mayor_id = 0, i = 0;
    t_deadlock *aux;
    char *retenido_a_liberar;
    char *esperando_a_sacar;
    sem_kernel *semaforo;
    for (i; i < list_size(lista_deadlock); i++)
    {
        aux = list_get(lista_deadlock, i);
        if (mayor_id < aux->pid)
        {
            mayor_id = aux->pid;
            strcpy(retenido_a_liberar, aux->retenido);
            strcpy(esperando_a_sacar, aux->esperando);
        }
    }
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
