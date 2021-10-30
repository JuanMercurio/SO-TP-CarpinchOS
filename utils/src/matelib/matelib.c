
#include "matelib.h"

//------------------General Functions---------------------/

int mate_init(mate_instance *lib_ref, char *config)
{
  t_config *configuracion = config_create(config);
  char *IP = config_get_string_value(configuracion, "IP");
  char *PUERTO = config_get_string_value(configuracion, "PUERTO");

 // lib_ref->group_info = malloc(sizeof(mate_inner_structure));
  mate_inner_structure mate_ref;
  lib_ref->info= mate_ref;

  lib_ref->conexion = crear_conexion(IP, PUERTO);
  char *respuesta = recibir_mensaje(lib_ref->conexion); 

  if(strcmp(respuesta, "KERNEL") == 0){
    lib_ref->info.con_kernel = true;
  } 
  else{
    lib_ref->info.con_kernel = false;
  }  
  
  enviar_mensaje_y_cod_op("NUEVO CARPINCHO", lib_ref->conexion, NEW_INSTANCE); // serializar structura
  respuesta = recibir_mensaje(lib_ref->conexion); 
  mate_ref.pid = atoi(respuesta);
  recibir_mensaje(lib_ref->conexion); // el procesos queda bloqueado aca hasta recibir un OK del procesador
  return 0;
}

int mate_close(mate_instance *lib_ref)
{
 free(lib_ref);
  return 0;
}

//-----------------Semaphore Functions---------------------/

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value) {
  
  if(lib_ref->info.con_kernel){

  //((mate_inner_structure *)lib_ref->group_info)->sem_instance = malloc(sizeof(sem_t));
 // sem_init(((mate_inner_structure *)lib_ref->group_info)->sem_instance, 0, value);
  enviar_sem_init(sem, (int)value, lib_ref->conexion, INIT_SEMAFORO); // codear fiuncion; pasar valor
  char* respuesta = recibir_mensaje(lib_ref->conexion); // espera respuesta para continuar ejecutando condigo???

  return 0;
  }
  return -1;//PREGUBNTAR POR RETORNO DE ENTERO
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem) {
  if(lib_ref->info.con_kernel){
    enviar_mensaje_y_cod_op(sem, lib_ref->conexion, SEM_WAIT);
  }
   // return sem_wait(((mate_inner_structure *)lib_ref->group_info)->sem_instance);
 }

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem) {
  if(lib_ref->info.con_kernel){
  enviar_mensaje_y_cod_op(sem, lib_ref->conexion, SEM_POST);
}
}
int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem) {
  if(lib_ref->info.con_kernel){
  enviar_mensaje_y_cod_op(sem, lib_ref->conexion, SEM_DESTROY);
}
}
//--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg)
{
  printf("Doing IO %s...\n", io);
  usleep(10 * 1000);
  if (!strncmp(io, "PRINTER", 7))
  {
    printf("Printing content: %s\n", (char *)msg);
  }
  printf("Done with IO %s\n", io);
  return 0;
}

//--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size)
{
  ((mate_inner_structure *)lib_ref->info)->memory = malloc(size);
  return 0;
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr)
{
  if (addr != 0)
  {
    return -1;
  }
  free(((mate_inner_structure *)lib_ref->info)->memory);
  return 0;
}

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size)
{
  if (origin != 0)
  {
    return -1;
  }
  memcpy(dest, ((mate_inner_structure *)lib_ref->info)->memory, size);
  return 0;
}

int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size)
{
  if (dest != 0)
  {
    return -1;
  }
  memcpy(((mate_inner_structure *)lib_ref->info)->memory, origin, size);
  return 0;
}

//--------------------Auxiliar Functions-----------------//

