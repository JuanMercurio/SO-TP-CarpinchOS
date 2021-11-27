#include "matelib.h"

//------------------General Functions---------------------/

/* 
  - recibe un mensaje (handshake)
  - envia NEW_INSTANCE
  - recibe un pid
  - recibe confirmacion 
        - "OK"    --> true
        - != "OK" --> false

 */
int mate_init(mate_instance *lib_ref, char *config) //AGREGAR LOG
{
  t_config *configuracion = config_create(config);
  char *IP = config_get_string_value(configuracion, "IP");
  char *PUERTO = config_get_string_value(configuracion, "PUERTO");
  char *ARCHIVO_LOG = config_get_string_value(configuracion, "ARCHIVO_LOG");

  logger = log_create(ARCHIVO_LOG, "Matelib", 0, LOG_LEVEL_INFO);
  log_info(logger, "Log Matelib creado");

  mate_inner_structure *mate_ref = malloc(sizeof(mate_inner_structure));
  lib_ref->group_info = mate_ref;

  mate_ref->conexion = crear_conexion(IP, PUERTO);

  log_info(logger, "Esperando respuesta de servidor");
  char *respuesta = recibir_mensaje(mate_ref->conexion);
  conexion_set_server(mate_ref, respuesta, logger);

  log_info(logger, "NUEVO CARPINCHO");
  enviar_int(mate_ref->conexion, NEW_INSTANCE);
  mate_ref->pid  = recibir_int(mate_ref->conexion);

  char* estado = recibir_mensaje(mate_ref->conexion); // el procesos queda bloqueado aca hasta recibir un OK del procesador

  if (strcmp(estado, "OK") == 0)
  {
    log_info(logger, "Carpincho creado correctamente con el pid: %d", mate_ref->pid);
    return 0;
  }
  else
  {
    log_info(logger, "Error al crear el carpimcho");
    return -1;
  }
}

int mate_close(mate_instance *lib_ref)
{
  log_info(logger, "MATE_CLOSE");
  //enviar_mensaje_y_cod_op("ELIMINAME",((mate_inner_structure*)lib_ref->group_info)->conexion, MATE_CLOSE);
  free(lib_ref->group_info);
  free(lib_ref);
  char *respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion);
  if (strcmp(respuesta, "OK") == 0)
  {
    log_info(logger, "Carpincho eliminado");
    return 0;
  }
  else
  {
    return -1;
  }
}

//-----------------Semaphore Functions---------------------/

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value)
{

  log_info(logger, "SEM_INIT");

  if (conectado_a_memoria(lib_ref))
  {
    log_info(logger, "Conectado con memoria. No se puede inicializar semáforo.");
    return -1;
  }

  //((mate_inner_structure *)lib_ref->group_info)->sem_instance = malloc(sizeof(sem_t));
  // sem_init(((mate_inner_structure *)lib_ref->group_info)->sem_instance, 0, value);

  enviar_sem_init(sem, (int)value, ((mate_inner_structure *)lib_ref->group_info)->conexion, INIT_SEMAFORO); // codear fiuncion; pasar valor
  char *respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion);               // espera respuesta para continuar ejecutando condigo???

  if (strcmp(respuesta, "OK") == 0)
  {
    log_info(logger, "Se creó correctamente el semáforo %s", sem);
    return 0;
  }
  else
  {
    log_info(logger, "Error al crear el semáforo %s", sem);
    return -1;
  }
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem)
{
  log_info(logger, "SEM_WAIT al semáforo %s", sem);

  if (conectado_a_memoria(lib_ref))
  {
    log_info(logger, "Conectado con memoria. No se puede hacer SEM WAIT.");
    return -1;
  }

  enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_WAIT);
  char *respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion);

  if (strcmp(respuesta, "OK") == 0)
  {
    log_info(logger, "Se realizó el WAIT correctamente");
    return 0;
  }
  else
  {
    log_info(logger, "Error al realizar el WAIT");
    return -1;
  }
}

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem)
{
  log_info(logger, "SEM_POST al semáforo %s", sem);

  if (conectado_a_memoria(lib_ref))
  {
    log_info(logger, "Conectado con memoria. No se puede hacer SEM POST.");
    return -1;
  }

  enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_POST);
  char *respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion);

  if (strcmp(respuesta, "OK") == 0)
  {
    log_info(logger, "Se realizó el POST correctamente");
    return 0;
  }
  else
  {
    log_info(logger, "Error al realizar el POST");
    return -1;
  }
}

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem)
{
  log_info(logger, "SEM_DESTROY al semáforo %s", sem);

  if (conectado_a_memoria(lib_ref))
  {
    log_info(logger, "Conectado con memoria. No se puede hacer SEM DESTROY.");
    return -1;
  }

  enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_DESTROY);
  char *respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion);

  if (strcmp(respuesta, "OK") == 0)
  {
    log_info(logger, "Se realizó el DESTROY correctamente");
    return 0;
  }
  else
  {
    log_info(logger, "Error al realizar el DESTROY");
    return -1;
  }
}
//--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg)
{
  log_info(logger, "CALL_IO a %s", io);

  if (conectado_a_memoria(lib_ref))
  {
    log_info(logger, "Conectado con memoria. No se puede hacer CALL_IO.");
    return -1;
  }

  enviar_mensaje_y_cod_op(io, ((mate_inner_structure *)lib_ref->group_info)->conexion, IO);
  char *respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion);

  if (strcmp(respuesta, "OK") == 0)
  {
    log_info(logger, "Se realizó el CALL IO correctamente");
    return 0;
  }
  else
  {
    log_info(logger, "Error al realizar el CALL IO");
    return -1;
  }
}

//--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size)
{
  log_info(logger, "MEM_ALLOC de %d", size);

  //((mate_inner_structure *)lib_ref->group_info)->memory = malloc(size);

  enviar_cod_op_e_int(((mate_inner_structure *)lib_ref->group_info)->conexion, MEMALLOC, size);
  recibir_operacion(((mate_inner_structure *)lib_ref->group_info)->conexion);

  int32_t respuesta = (int32_t)recibir_operacion(((mate_inner_structure *)lib_ref->group_info)->conexion);

  if ((int)respuesta == -1)
  {
    log_info(logger, "Error al realizar el MEM_ALLOC");
    return -1;
  }
  else
  {
    log_info(logger, "Se realizó el MEM_ALLOC correctamente. La dirección lógica es %d", respuesta);
    return respuesta;
  }
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr)
{
  log_info(logger, "MEM_FREE a la dirección %d", addr);

  enviar_cod_op_e_int(((mate_inner_structure *)lib_ref->group_info)->conexion, MEMFREE, addr);

  int respuesta = recibir_operacion(((mate_inner_structure *)lib_ref->group_info)->conexion);

  if (respuesta == -5)
  {
    log_info(logger, "Error al realizar el MEM_FREE");

    return MATE_FREE_FAULT;
  }
  else
  {
    log_info(logger, "Se realizó el MEM_FREE correctamente");

    return respuesta;
  }
}

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size) // revisar que retorna
{
  log_info(logger, "MEM_READ desde %d el size %d", origin, size);

  enviar_mem_read(((mate_inner_structure *)lib_ref->group_info)->conexion, MEMREAD, (int)origin, size);

  dest = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion);

  if (strcmp(dest, "-6") == 0)
  {
    log_info(logger, "Error al realizar el MEM_READ");
    dest = NULL;
    return MATE_READ_FAULT;
  }
  else
  {
    log_info(logger, "Se realizó el MEM_READ correctamente. El contenido es: %s", (char *)dest);

    return 0;
  }
}

int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size)
{
  log_info(logger, "MEM_WRITE el mensaje %s en %d", (char *)origin, dest);

  enviar_mem_write(((mate_inner_structure *)lib_ref->group_info)->conexion, MEMREAD, origin, (int)dest, size);
  int recibido = recibir_operacion(((mate_inner_structure *)lib_ref->group_info)->conexion);

  if (recibido == -7)
  {
    log_info(logger, "Error al realizar MEM_WRITE");

    return MATE_WRITE_FAULT;
  }
  else
  {
    log_info(logger, "Se realizó el MEM_WRITE correctamente");
    return recibido;
  }
}

/*
ESTAS FUNCIONES DE MEMEMORIA DEBEN TENER LA ESPERA DE UNA CONFIRMACION POR PARTE DEL MODULO PARA SEGUIR EJECUTADO
INDEPENDIENTEMENTE DEL RETURN PARA EVITA QUE EL PROCEOS HAGA OTRA PETICION Y AUN NO HAYA TERMINADO DE HACER LO ANTERIOR
 */
//--------------------Auxiliar Functions-----------------//

// funcion al pedo pero sirve para lectura
int obtener_int(mate_instance *lib_ref, cod_int COD)
{
  void *buffer = leer_bloque(lib_ref->group_info, sizeof(int) * COD, sizeof(int));
  return *(int *)buffer;
}

// funcion al pedo pero sirve para lectura
void *leer_bloque(void *bloque, int offset, int tamanio)
{
  void *buffer = malloc(tamanio);
  memcpy(buffer, bloque + offset, tamanio);
  return buffer;
}

void *crear_mate_inner(int pid, int conexion, int server)
{

  void *bloque = malloc(sizeof(int) * 3 + sizeof(sem_t)); // verificar tamanio para semaforos
  int offset = 0;

  memcpy(bloque + offset, &pid, sizeof(int));
  offset += sizeof(int);
  memcpy(bloque + offset, &conexion, sizeof(int));
  offset += sizeof(int);
  memcpy(bloque + offset, &server, sizeof(int));
  offset += sizeof(int);

  return bloque;
}

mate_pointer recibir_dl(int socket)
{
  return recibir_int32(socket);
}

mate_pointer recibir_int32(int socket)
{
  void *buffer = malloc(sizeof(mate_pointer));
  recv(socket, buffer, sizeof(mate_pointer), 0);
  mate_pointer dl = *(mate_pointer *)buffer;
  free(buffer);
  return dl;
}

bool conectado_a_memoria(mate_instance *lib_ref)
{
  return !((mate_inner_structure *)lib_ref->group_info)->con_kernel;
}



void conexion_set_server(mate_inner_structure *mate_ref, char* respuesta, t_log* logger)
{ 
  if (strcmp(respuesta, "KERNEL") == 0)
    mate_ref->con_kernel = true;
  else
    mate_ref->con_kernel = false;

  char mensaje[50];
  sprintf(mensaje, "Conectado a %s", respuesta);
}
