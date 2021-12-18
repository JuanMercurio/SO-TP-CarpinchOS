#include "deadlock.h"
#include "../io_semaforos/io_semaforos.h"
#include "../planificacion/planificacion.h"

void deadlock()
{
    while (!terminar)
    {
        printf("---------------------------------------DEADLOCK ACTIVADO\n");
        sleep(configuracion.TIEMPO_DEADLOCK * 0.001);
        deteccion_deadlock();
    }
    sem_post(&semaforo_sicro_fin_deadlock);
}
void deteccion_deadlock()
{   if(terminar){
    printf( "SALE POR IF DE TERMINAR\n");
    return;
}else{
    sem_kernel *semaforo;
    deadlock_kernel *a_enlistar = NULL;
    printf("---------------------------------------COMIENZA DETECCION DE DEADLOCK\n");
    t_list *lista_posible_deadlock = list_create();
    printf("TAMAÑO POSIBLE DEADLOCK %d\n", list_size(lista_posible_deadlock));
    t_list *lista_con_deadlock;
    log_info(logger, "Comienza la ejecución de la detección de Deadlock");
    if (!list_is_empty(lista_sem_kernel))
    {
        for (int i = 0; i < list_size(lista_sem_kernel); i++)
        {
            log_info(logger, "Entro al for deadlock");
            printf("Entro al for deadlock tamaño lista de semaforos %d\n", list_size(lista_sem_kernel));
            sem_wait(&mutex_lista_sem_kernel);
            semaforo = (sem_kernel *)list_get(lista_sem_kernel, i);
            printf("saco semaforo %s tomado por %d\n", semaforo->id, semaforo->tomado_por);
            sem_post(&mutex_lista_sem_kernel);
            if (semaforo->tomado_por != -1){
                printf("entro a tomado por alguien \n");
                printf("Semaforo->tomado_por %d\n", semaforo->tomado_por);
                 a_enlistar = buscar_en_otras_listas(semaforo->tomado_por, i, semaforo->id);
            }else{
                a_enlistar = NULL;
                 }
         //  printf(" !!!!!saco de la lista carpincho %d con retenido %s y esperando %s\n", a_enlistar->pid, a_enlistar->retenido, a_enlistar->esperando);
            if (a_enlistar != NULL)
            {
                list_add(lista_posible_deadlock, (void *)a_enlistar);
                printf("enlisto posible carpincho con deadlock\n");
            }
            else
            {
                free(a_enlistar);
            }
            //log_info(logger, "Termine iteracion %d del for deadlock", i);
        }
        log_info(logger, "Finaliza la búsqueda de carpinchos en posible deadlock");
        printf("Finaliza la búsqueda de carpinchos en posible deadlock\n");
        //printf("Size lista_posible_deadlock %d\n", list_size(lista_posible_deadlock));
        if (!list_is_empty(lista_posible_deadlock))
        {
            printf("Verificando espera circular con %d elementos\n", list_size(lista_posible_deadlock));
            log_info(logger, "Verificando espera circular");
            lista_con_deadlock = verificar_espera_circular(lista_posible_deadlock);
            while (lista_con_deadlock != NULL)
            {
                printf("DEADLOCK finalizara involucrados\n");
                //if (lista_con_deadlock != NULL)
                finalizar_involucrados(lista_con_deadlock);
                printf("DEADLOCK destruye los involucrados restantes de la lista con deadlock\n");
                if (!list_is_empty(lista_con_deadlock))
                    list_destroy_and_destroy_elements(lista_con_deadlock, lista_deadlock_destroyer);
                else
                {
                    printf("dalleeeee\n");
                    list_destroy(lista_con_deadlock);
                }

                printf("DEADLOCK destruye los involucrados restantes de la lista con deadlock\n");
                lista_con_deadlock = verificar_espera_circular(lista_posible_deadlock);
            }
            printf("DESTUYENDO LISTA DE POSIBLE DEADLOCK\n");
            log_info(logger, "Destruyendo lista de posible deadlock");
            if(list_is_empty(lista_posible_deadlock))
            list_destroy(lista_posible_deadlock);
            else
            {
                list_destroy_and_destroy_elements(lista_posible_deadlock, lista_deadlock_destroyer);
            }
            
            printf("destruyo lista posible deadolock sin elementos\n");
        }
        else
        {
            printf("No se encontraron carpinchos en posible deadlock\n");
            log_info(logger, "No se encontraron carpinchos en posible deadlock");
            log_info(logger, "Destruyendo lista de posible deadlock vacia");
            printf("DESTUYENDO LISTA DE POSIBLE DEADLOCK VACIA\n");
            list_destroy(lista_posible_deadlock);
        }
    }
    else
        list_destroy(lista_posible_deadlock);
}
}

void lista_deadlock_destroyer(void *arg)
{
    printf("ESNTYRO A DESTRUIR LISTAAAAAA\n");
    deadlock_kernel *a_destruir = (deadlock_kernel *)arg;
    printf("ESNTYRO A DESTRUIR LISTAAAAAA\n");
    printf("destruyendo carpincho %d \n", a_destruir->pid);
    free(a_destruir->retenido);
    free(a_destruir->esperando);
    free(a_destruir);
    printf("saliendo de destruir un carpin deadlock DESTRUIR LISTAAAAAA\n");
}
deadlock_kernel *buscar_en_otras_listas(int pid, int index, char *semaforo_retenido)
{ //log_info(logger, "Estoy en buscar_en_otras_listas con carpincho %d", pid);
    t_pcb *carpincho_bloqueado;
    int i = 0, j;
    sem_kernel *semaforo;
    bool encontrado = false;
    while (i < list_size(lista_sem_kernel) && !encontrado)
    {
        printf("entro a while de lista semaforos con pid %d\n", pid);
      /*   if (i == index)
        {
            printf(" valor de i :%d\n", i);
            i++;
            continue;
        } */
        semaforo = (sem_kernel *)list_get(lista_sem_kernel, i);
        j = 0;
        while (j < list_size(semaforo->bloqueados) && !encontrado)
        {
            printf("buscar_en_otras_listas ---- la cola NO está vacía --- size %d\n", list_size(semaforo->bloqueados));
            //log_info(logger,"buscar_en_otras_listas ---- la cola NO está vacía --- size %d", list_size(semaforo->bloqueados));
            carpincho_bloqueado = (t_pcb *)list_get(semaforo->bloqueados, j); //TIENE QUE SER UNA LISTA;
            printf("LISTA DE BLOQUEADOS DE SEMAFORO %s CON ELEMENTOS %d\n ", semaforo->id, list_size(semaforo->bloqueados));
            // log_info(logger, "buscar_en_otras_listas - carpincho que recibe %d - carpincho que saca de bloqueado %d", pid, carpincho_bloqueado->pid);
            printf("pid %d y carpincho_bloqueado pid %d\n", pid , carpincho_bloqueado->pid);
            if (pid == carpincho_bloqueado->pid)
            {
                printf("encontro algo con pid!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %d\n ", pid);
                // log_info(logger, "El carpincho %d está en posible deadlock - Semáforo retenido: %s - Semáforo por el que se bloqueó: %s", pid, semaforo_retenido, semaforo->id);
                deadlock_kernel *carpincho_deadlock = (deadlock_kernel *)malloc(sizeof(deadlock_kernel));
                carpincho_deadlock->pid = pid;
                carpincho_deadlock->retenido = string_duplicate(semaforo_retenido);
                carpincho_deadlock->esperando = string_duplicate(semaforo->id);
                encontrado = true;
                printf(" Retornando carpincho %d con retenido %s y esperando %s\n", carpincho_deadlock->pid, carpincho_deadlock->retenido, carpincho_deadlock->esperando);
                return carpincho_deadlock;
                
            }
            else
            {
                printf("DEVUELVE NULL POR SEAMFORO TOMADO POR %d\n",pid);
                //return NULL;
            }
            j++;
        }
        i++;
    } if(!encontrado)
    printf("-----------------DEVUELVE NULL POR NO ENCONTRADO");
        return NULL; 
}

t_list *verificar_espera_circular(t_list *lista)
{
    t_list *lista_aux = list_create();
    bool cerrado = false;
    //printf("en espera circular\n");
    while (!list_is_empty(lista) && !cerrado)
    {      
        printf("---------------------------antes de remover de posible deadlock con elementos %d\n", list_size(lista));
       // printf(" saco de la lista carpincho %d con retenido %s y esperando %s\n", a_evaluar->pid, a_evaluar->retenido, a_evaluar->esperando);
       deadlock_kernel *a_evaluar = (deadlock_kernel*) list_remove(lista, 0);
        printf("struct deadlock removido de lista de poisble deadlock: carpincho: %d\n", a_evaluar->pid);
        if (a_evaluar->pid > 0)
        {
            printf("despues de remover antes de encontrar circulo\n");
            cerrado = encontrar_circulo(a_evaluar, lista, lista_aux);
        }
    }
    if (cerrado)
    {
        printf("ENCONTRO CIRCULOOOOOOO DE %d ELEMENTOS\n", list_size(lista_aux));
        log_info(logger, "Espera circular detectada");
        return lista_aux;
    }
    else
    {
        if(list_is_empty(lista_aux)) 
            list_destroy(lista_aux);
        else {
            list_destroy_and_destroy_elements(lista_aux, lista_deadlock_destroyer);
        }
        printf("No se detectó espera circular");
        log_info(logger, "No se detectó espera circular");
        return NULL;
    }
}
//--------------------------------------------------------------
bool encontrar_circulo(deadlock_kernel *a_evaluar, t_list *lista, t_list *lista_aux)
{
    list_add(lista_aux, (void *)a_evaluar);
    int *index = malloc(sizeof(int));
    printf("entro a encontrar circulo\n");
    if (comparar_lista(a_evaluar, lista_aux, index)) // busca en la auxiliar si se cierra con atenriores
    {                                                //printf(" encontrar circulo: busca en lista auxiliar a capincho %d\n", a_evaluar->pid);
        free(index);
        return true;
    }
    if (comparar_lista(a_evaluar, lista, index))
    {
        printf(" encontrar circulo: busca en lista de posibles deadlocka capincho %d\n", a_evaluar->pid);
        a_evaluar = list_remove(lista, *index);
        free(index);
        encontrar_circulo(a_evaluar, lista, lista_aux);
    }
    else
    {
        free(index);
        return false;
    }
}

//------------------------------------------------------
bool comparar_lista(deadlock_kernel *alf, t_list *list, int *index)
{
    deadlock_kernel *aux;
    int i = 0;
    bool encontro = false;

    printf("entro a comparar lista\n");
    if (!list_is_empty(list))
    {
        printf("tamanio de lista %d\n", list_size(list));

        while (i < list_size(list) && !encontro)
        {
            printf("entro a while\n");
            aux = list_get(list, i);
            printf("desenlisto\n");
            printf("desenlisto %d con esperando %s\n", aux->pid, aux->esperando);
            if (strcmp(alf->esperando, aux->retenido) == 0)
            {
                *index = i;
                encontro = true;
                printf("encontro\n");
            }
            i++;
            printf("i = %d\n", i);
        }
        printf("sale de compara lista\n");
        return encontro;
    }
    else
    {
        return false;
    }
    free(aux->esperando);
    free(aux->retenido);
    free(aux);
}

void finalizar_involucrados(t_list *lista_deadlock)
{
    int mayor_id = 0;
    deadlock_kernel *aux;
    sem_kernel *semaforo;
    int i = 0;
    int posicion=0;
   // bool encontrado = false;
    printf("--------------------Tamaño lista con deadlock %d\n",list_size(lista_deadlock));
    while (i < list_size(lista_deadlock) )
    {
        aux = list_get(lista_deadlock, i);
        
        if (mayor_id < aux->pid)
        {
            mayor_id = aux->pid;
            posicion =i;
    
            } i++;
    }
            aux = list_get(lista_deadlock, posicion);
            char *retenido_a_liberar = (char *)malloc(strlen(aux->retenido) + 1);
            char *esperando_a_sacar = (char *)malloc(strlen(aux->esperando) + 1);
            strcpy(retenido_a_liberar, aux->retenido);
            strcpy(esperando_a_sacar, aux->esperando);
            printf("Se finaliza el carpincho PID %d\n", aux->pid);
            log_info(logger, "Se finaliza el carpincho PID %d", mayor_id);
            int pos;
            printf("busca el semaforo para desbloquear\n");
            semaforo = buscar_semaforo2(esperando_a_sacar, &pos);
            sacar_de_cola_bloqueados(semaforo, mayor_id);
            sem_kernel_post(retenido_a_liberar);
            sem_kernel_post(esperando_a_sacar);
            free(retenido_a_liberar);
            free(esperando_a_sacar);
}

void sacar_de_cola_bloqueados(sem_kernel *semaforo, int id)
{
    t_pcb *carpincho;
    int i = 0;
    bool removido = false;
    while (i < list_size(semaforo->bloqueados) && !removido)
    {
        sem_wait(&semaforo->mutex_cola);
        carpincho = (t_pcb *)list_get(semaforo->bloqueados, i);
        sem_post(&semaforo->mutex_cola);
        if (carpincho->pid == id)
        {
            printf("carpincho %d se trabo en wait\n", carpincho->pid);
            sem_wait(&semaforo->mutex_cola);
            printf("carpincho %d con loe liminaara lista\n", carpincho->pid);
            list_remove(semaforo->bloqueados, i);
            sem_post(&semaforo->mutex_cola);
            log_info(logger, "carpincho %d con deadlock removid de lista\n", carpincho->pid);
            printf("carpincho %d con deadlock removido de lista\n", carpincho->pid);
            enviar_mensaje(carpincho->fd_cliente, "OK");
            carpincho->pid = -1;
            removido = true;
        }
        i++;
    }
}
