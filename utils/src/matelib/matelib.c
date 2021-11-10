#include "matelib.h"

//------------------General Functions---------------------/

void mate_init(mate_instance *lib_ref, char *config)
{
  t_config *configuracion = config_create(config);
  char *IP = config_get_string_value(configuracion, "IP");
  char *PUERTO = config_get_string_value(configuracion, "PUERTO");

  int pid = INIT;
  int conexion = crear_conexion(IP, PUERTO);
  int server =  recibir_int(conexion);

  enviar_int(conexion, NEW_INSTANCE);

  // No se si hace falta iniciar el sem_instance

  pid = recibir_PID(conexion);

  // Valida si fue validad por el kernel o la memoria
  conexion_success(pid); 

  lib_ref->group_info = crear_mate_inner(pid, conexion, server);
}

void conexion_success(int pid){
  if(pid == NOT_ASIGNED) abort();    //definir comportamiento.. si no es aceptado en el sistema vuelve a intentar? retorna -1?
}

int mate_close(mate_instance *lib_ref)
{
  //enviar_mensaje_y_cod_op("ELIMINAME",((mate_inner_structure*)lib_ref->group_info)->conexion, MATE_CLOSE);
  free(lib_ref->group_info);
  free(lib_ref);
  return 0;
}

//-----------------Semaphore Functions---------------------/

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value) {
  
  if(((mate_inner_structure *)lib_ref->group_info)->conectado_a == MEMORIA) return -1;

  //((mate_inner_structure *)lib_ref->group_info)->sem_instance = malloc(sizeof(sem_t));
  // sem_init(((mate_inner_structure *)lib_ref->group_info)->sem_instance, 0, value);

  enviar_sem_init(sem, (int)value, ((mate_inner_structure *)lib_ref->group_info)->conexion, INIT_SEMAFORO); // codear fiuncion; pasar valor
  char* respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion); // espera respuesta para continuar ejecutando condigo???

  return 0;
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem) {
  if(conectado_a_memoria(lib_ref)) return -1;

  enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_WAIT);
  return 0;
  // return sem_wait(((mate_inner_structure *)lib_ref->group_info)->sem_instance);
 }

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem) {
  if(conectado_a_memoria(lib_ref)) return -1;

  enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_POST);
  return 0;
}

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem) {
  if(conectado_a_memoria(lib_ref)) return -1;

  enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_DESTROY);
  return 0;
}
//--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg)
{
  enviar_mensaje_y_cod_op(io, ((mate_inner_structure*)lib_ref->group_info)->conexion, IO);
  char* respuesta = recibir_mensaje(((mate_inner_structure*)lib_ref->group_info)->conexion);
  if(strcmp(respuesta, "OK") == 0){
    return 0;
  }
  return -1;
}

//--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size)
{
  mate_pointer dl = NOT_ASIGNED;

  int socket = obtener_int(lib_ref, SOCKET);
  int pid = obtener_int(lib_ref, PID);
  printf("voy a mandar el pid: %d\n", pid);

  solicitud_malloc(socket, pid, size);
  dl = recibir_dl(socket);

  return dl;
}

void solicitud_malloc(int socket, int pid, int size){

  t_paquete* paquete = crear_paquete(MEMALLOC);
  
  agregar_a_paquete(paquete, &pid, sizeof(int));
  agregar_a_paquete(paquete, &size, sizeof(int));

  enviar_paquete(paquete, socket);

  eliminar_paquete(paquete);
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr)
{
  return 0;
}

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size)
{
  if (origin != 0)
  {
    return -1;
  }
  //memcpy(dest, ((mate_inner_structure *)lib_ref->groupinfo)->memory, size);
  return 0;
}

int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size)
{
  if (dest != 0)
  {
    return -1;
  }
  //memcpy(((mate_inner_structure *)lib_ref->info)->memory, origin, size);
  return 0;
}
/*
ESTAS FUNCIONES DE MEMEMORIA DEBEN TENER LA ESPERA DE UNA CONFIRMACION POR PARTE DEL MODULO PARA SEGUIR EJECUTADO
INDEPENDIENTEMENTE DEL RETURN PARA EVITA QUE EL PROCEOS HAGA OTRA PETICION Y AUN NO HAYA TERMINADO DE HACER LO ANTERIOR
 */
//--------------------Auxiliar Functions-----------------//


// funcion al pedo pero sirve para lectura
int obtener_int(mate_instance* lib_ref, cod_int COD){
  void* buffer = leer_bloque(lib_ref->group_info, sizeof(int)*COD, sizeof(int));
  return *(int*)buffer;
}

// funcion al pedo pero sirve para lectura
void* leer_bloque(void* bloque, int offset, int tamanio){
  void* buffer = malloc(tamanio);
  memcpy(buffer, bloque + offset, tamanio);
  return buffer;
}

void* crear_mate_inner(int pid, int conexion, int server){

  void* bloque = malloc(sizeof(int)*3 + sizeof(sem_t)); // verificar tamanio para semaforos
  int offset = 0;

  memcpy(bloque + offset, &pid, sizeof(int));
  offset += sizeof(int);
  memcpy(bloque + offset, &conexion, sizeof(int));
  offset += sizeof(int);
  memcpy(bloque + offset, &server, sizeof(int));
  offset += sizeof(int);

  return bloque;
}

mate_pointer recibir_dl(int socket){
  return recibir_int32(socket);
}

mate_pointer recibir_int32(int socket){
  void* buffer = malloc(sizeof(mate_pointer));
  recv(socket, buffer, sizeof(mate_pointer), 0);
  mate_pointer dl = *(mate_pointer*)buffer;
  free(buffer);
  return dl;
}

bool conectado_a_memoria(mate_instance* lib_ref){
  return ((mate_inner_structure *)lib_ref->group_info)->conectado_a == MEMORIA;
}