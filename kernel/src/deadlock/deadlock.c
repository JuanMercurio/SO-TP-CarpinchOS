#include "deadlock.h"
#include "../io_semaforos/io_semaforos.h"
#include "../planificacion/planificacion.h"

void deteccion_deadlock()
{
    sem_kernel *semaforo;
    deadlock_kernel *a_enlistar;

    while (!terminar)
    {
        t_list *lista_posible_deadlock = list_create();
        printf("---------------------------------------DEADLOCK ACTIVADO\n");
        sleep(configuracion.TIEMPO_DEADLOCK * 0.001);
        printf("---------------------------------------PASARON LOS SEGUNDOS\n");
        log_info(logger, "Comienza la ejecución de la detección de Deadlock");
        //pausar_todo????
        if (!list_is_empty(lista_sem_kernel))
        {
            //printf("---------------------------------------LISTA NO VACIA BUSCARA TOMADOS POR\n");
            for (int i = 0; i < list_size(lista_sem_kernel); i++)
            {
                //log_info(logger, "Entro al for deadlock");
                sem_wait(&mutex_lista_sem_kernel);
                semaforo = (sem_kernel *)list_get(lista_sem_kernel, i);
                sem_post(&mutex_lista_sem_kernel);
                //printf("semaforo %s\n", semaforo->id);
                //printf("Semaforo->tomado_por %d", semaforo->tomado_por);
                if(semaforo->tomado_por != -1)
                // log_info(logger, "Semaforo->tomado_por %d", semaforo->tomado_por);
                a_enlistar = buscar_en_otras_listas(semaforo->tomado_por, i, semaforo->id);
                else
                a_enlistar = NULL;
                //log_info(logger, "Salgo de buscar_en_otras_listas con iteracion %d", i);
                if (a_enlistar != NULL)
                {
                    //log_info(logger, "Entro a if a_enlistar=!NULL");
                    list_add(lista_posible_deadlock, (void *)a_enlistar);
                    //printf("enlisto posible carpincho con deadlock\n");
                }
                else
                {
                    free(a_enlistar);
                }

                //log_info(logger, "Termine iteracion %d del for deadlock", i);
            }
            log_info(logger,"Finaliza la búsqueda de carpinchos en posible deadlock");
            printf("Finaliza la búsqueda de carpinchos en posible deadlock\n");
            //printf("Size lista_posible_deadlock %d\n", list_size(lista_posible_deadlock));
            if (!list_is_empty(lista_posible_deadlock))
            {
                printf("Verificando espera circular\n");
                log_info(logger, "Verificando espera circular");
                //log_info(logger, "Size lista_posible_deadlock %d", list_size(lista_posible_deadlock));
                t_list *lista_con_deadlock = verificar_espera_circular(lista_posible_deadlock);
                //printf("Size lista_posible_deadlock %d\n", list_size(lista_posible_deadlock));
                //printf("Size lista_con_deadlock %d\n", list_size(lista_con_deadlock));
                //  while (lista_con_deadlock != NULL)
                //{printf("DEADLOCK finalizara involucrados\n");
                if (lista_con_deadlock != NULL)
                    finalizar_involucrados(lista_con_deadlock);
                //lista_con_deadlock = verificar_espera_circular(lista_posible_deadlock);
                // }
            }
            else
            {
                printf("No se encontraron carpinchos en posible deadlock\n");
                log_info(logger,"No se encontraron carpinchos en posible deadlock");
            }
            if (!list_is_empty(lista_posible_deadlock))
            {
                printf("DESTUYENDO LISTA DE POSIBLE DEADLOCK\n");
                log_info(logger,"Destruyendo lista de posible deadlock");
                list_destroy_and_destroy_elements(lista_posible_deadlock, lista_deadlock_destroyer);
            }
            else
            {   log_info(logger,"Destruyendo lista de posible deadlock vacia");
                printf("DESTUYENDO LISTA DE POSIBLE DEADLOCK VACIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
                list_destroy(lista_posible_deadlock);
            }
        }
    }
}

    void lista_deadlock_destroyer(void *arg)
    {
        deadlock_kernel *a_destruir = (deadlock_kernel *)arg;
        free(a_destruir->retenido);
        free(a_destruir->esperando);
        free(a_destruir);
    }

    deadlock_kernel *buscar_en_otras_listas(int pid, int index, char *semaforo_retenido)
    {
        //log_info(logger, "Estoy en buscar_en_otras_listas con carpincho %d", pid);
        t_pcb *carpincho_bloqueado;
        int i = 0, j;
        sem_kernel *semaforo;
        bool encontrado = false;
        deadlock_kernel *carpincho_deadlock = malloc(sizeof(deadlock_kernel));
        //printf("creo struct tamanio %d\n", sizeof(deadlock_kernel));
        while (i < list_size(lista_sem_kernel) && !encontrado)
        {
            //printf("entro a while de lista semaforos\n");
            /*   if (i == index){
             printf("hace continue\n");
            continue;} */
            //printf(" valor de i :%d\n", i);
            semaforo = (sem_kernel *)list_get(lista_sem_kernel, i);
            //printf("buscar_en_otras_listas semaforo que saco %s\n", semaforo->id);
            j = 0;
            while (j < list_size(semaforo->bloqueados) && !encontrado)
            {
                //printf("buscar_en_otras_listas ---- antes de while  cola NO vacía --- size %d\n", list_size(semaforo->bloqueados));
                //log_info(logger,"buscar_en_otras_listas ---- la cola NO está vacía --- size %d", list_size(semaforo->bloqueados));
                carpincho_bloqueado = (t_pcb *)list_get(semaforo->bloqueados, j); //TIENE QUE SER UNA LISTA;
                //printf("buscar_en_otras_listas - carpincho que recibe %d - carpincho que saca de bloqueado %d\n", pid, carpincho_bloqueado->pid);
                // log_info(logger, "buscar_en_otras_listas - carpincho que recibe %d - carpincho que saca de bloqueado %d", pid, carpincho_bloqueado->pid);

                if (pid == carpincho_bloqueado->pid)
                {
                    printf("El carpincho %d está en posible deadlock - Semáforo retenido: %s - Semáforo por el que se bloqueó: %s\n", pid, semaforo_retenido, semaforo->id);
                    log_info(logger, "El carpincho %d está en posible deadlock - Semáforo retenido: %s - Semáforo por el que se bloqueó: %s", pid, semaforo_retenido, semaforo->id);

                    carpincho_deadlock->pid = pid;
                    printf("1\n");
                    carpincho_deadlock->retenido = string_duplicate(semaforo_retenido);
                    printf("2\n");
                    carpincho_deadlock->esperando = string_duplicate(semaforo->id);
                    printf("3\n");
                    encontrado = true;
                    printf("4\n");
                }
                else
                {
                    //carpincho_deadlock = NULL;
                }
                j++;
            }
            i++;
        }
        //printf("devolviendo carponcho en posible deadlock\n");
        return carpincho_deadlock;
    }
    t_list *verificar_espera_circular(t_list * lista)
    {
        t_list *lista_aux = list_create();
        bool cerrado = false;
        //printf("en espera circular\n");
        while (!list_is_empty(lista) &&!cerrado)
        {
            deadlock_kernel *a_evaluar = list_remove(lista, 0);
            //printf("struct deadlock removido de lista de poisble deadlock: carpincho: %d\n", a_evaluar->pid);
            if(a_evaluar->pid > 0){
                cerrado = encontrar_circulo(a_evaluar, lista, lista_aux);
            }
            
        }
    if (cerrado)
    {
        printf("ENCONTRO CIRCULOOOOOOO DE %d ELEMENTOS\n", list_size(lista_aux));
        log_info(logger, "Espera circular detectada");
        return lista_aux;
    }
    else{
        printf("No se detectó espera circular");  
        log_info(logger, "No se detectó espera circular");    
        return NULL;
    }
}
//--------------------------------------------------------------
bool encontrar_circulo(deadlock_kernel *a_evaluar, t_list *lista, t_list *lista_aux)
{
    int i =0;
    list_add(lista_aux, (void*) a_evaluar);
    int *index = malloc(sizeof(int));
    //printf("entro a encontrar circulo\n");
    if (comparar_lista(a_evaluar, lista_aux, index))// busca en la auxiliar si se cierra con atenriores
       {//printf(" encontrar circulo: busca en lista auxiliar a capincho %d\n", a_evaluar->pid);

    /*     while(i <*index)
        {   
            list_remove_and_destroy_element(lista_aux, i, lista_deadlock_destroyer);
            
        } */
        //printf("VA A DEVOLVER UNA LISTA CON %d ELEMENTOS\n", list_size(lista_aux));
        free(index);
        return true;
    }
        if (comparar_lista(a_evaluar, lista, index))
        {
            //printf(" encontrar circulo: busca en lista de posibles deadlocka capincho %d\n", a_evaluar->pid);
            a_evaluar = list_remove(lista, *index);//-------
            encontrar_circulo(a_evaluar, lista, lista_aux);
        }else{
        free(index);
        return false;
// aca se rompio
        }

    
}

//------------------------------------------------------
bool comparar_lista(deadlock_kernel *alf, t_list *list, int *index)
{
    deadlock_kernel *aux;
    int i = 0;
    bool encontro = false;
    //printf("entro a comparar lista\n");
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
                i++;
        }
        return encontro;
    }
    else
    {
        return false;
    }
    //agrego frees
    free(aux->esperando);
    free(aux->retenido);
    free(aux);
}

void finalizar_involucrados(t_list *lista_deadlock)
{
    int mayor_id = 0;
    deadlock_kernel *aux;
    char *retenido_a_liberar;
    char *esperando_a_sacar;
    sem_kernel *semaforo;
    for (int i=0; i < list_size(lista_deadlock); i++)
    {
        aux = list_get(lista_deadlock, i);
        if (mayor_id < aux->pid)
        {
            mayor_id = aux->pid;
            retenido_a_liberar = string_duplicate(aux->retenido);
            esperando_a_sacar = string_duplicate(aux->esperando);
        }
    }
    printf("Se finaliza el carpincho PID %d", mayor_id);
    log_info(logger, "Se finaliza el carpincho PID %d", mayor_id);
    int pos;
    printf("busca el semaforo para desbloquear\n");
    semaforo = buscar_semaforo2(esperando_a_sacar, &pos);
    sacar_de_cola_bloqueados(semaforo, mayor_id);
   
    printf("se manda posst a semaforo %s\n", retenido_a_liberar);
    sem_kernel_post(retenido_a_liberar);
    sem_kernel_post(esperando_a_sacar);
    list_destroy_and_destroy_elements(lista_deadlock, lista_deadlock_destroyer);

    //agrego frees
    free(retenido_a_liberar);
    free(esperando_a_sacar);
}

void sacar_de_cola_bloqueados(sem_kernel *semaforo, int id)
{
    t_pcb *carpincho;
    int i = 0;
    bool removido =false;
    while (i < list_size(semaforo->bloqueados) && !removido)
    {
        sem_wait(&semaforo->mutex_cola);
        carpincho = (t_pcb *)list_get(semaforo->bloqueados,i);
        sem_post(&semaforo->mutex_cola);
        if (carpincho->pid == id)
        {
           list_remove(semaforo->bloqueados,i);
            log_info(logger,"carpincho %d con deadlock removid de lista\n", carpincho->pid);
           printf("carpincho %d con deadlock removid de lista\n", carpincho->pid);
          // enviar_int(carpincho->fd_cliente, 0);
           enviar_mensaje(carpincho->fd_cliente, "OK");
          // close(carpincho->fd_cliente);
         // shutdown(carpincho->fd_cliente, SHUT_RD);
           eliminar_carpincho((void*)carpincho);
           removido = true;
         /* close(carpincho->fd_cliente);
            close(carpincho->fd_memoria); 
            sem_wait(&mutex_cola_finalizados);
            queue_push(cola_finalizados, (void *)carpincho);
            sem_post(&mutex_cola_finalizados);
            sem_post(&cola_finalizados_con_elementos); */
        }
        i++;
}
}
