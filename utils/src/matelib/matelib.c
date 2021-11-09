
#include "matelib.h"

//------------------General Functions---------------------/

int mate_init(mate_instance *lib_ref, char *config)
{
  t_config *configuracion = config_create(config);
  char *IP = config_get_string_value(configuracion, "IP");
  char *PUERTO = config_get_string_value(configuracion, "PUERTO");

  lib_ref->group_info = malloc(sizeof(mate_inner_structure));
  mate_inner_structure mate_ref;
  lib_ref->group_info= &mate_ref;

((mate_inner_structure *)lib_ref->group_info)->conexion = crear_conexion(IP, PUERTO);
  char *respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion); 
 //TOOD: debera retronar -1 si por algo no se puede crear la instancia??
  if(strcmp(respuesta, "KERNEL") == 0){
   ((mate_inner_structure *)lib_ref->group_info)->con_kernel = true;
  } 
  else{
    ((mate_inner_structure *)lib_ref->group_info)->con_kernel = false;
  }  
  
  enviar_mensaje_y_cod_op("NUEVO CARPINCHO", ((mate_inner_structure *)lib_ref->group_info)->conexion, NEW_INSTANCE);
  respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion); 
  mate_ref.pid = atoi(respuesta);
  recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion); // el procesos queda bloqueado aca hasta recibir un OK del procesador
  return 0;
}

int mate_close(mate_instance *lib_ref)
{
  enviar_mensaje_y_cod_op("ELIMINAME",((mate_inner_structure*)lib_ref->group_info)->conexion, MATE_CLOSE);
  free(lib_ref->group_info);
  free(lib_ref);
  return 0;
}

//-----------------Semaphore Functions---------------------/

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value) {
  
  if(((mate_inner_structure *)lib_ref->group_info)->con_kernel){

  //((mate_inner_structure *)lib_ref->group_info)->sem_instance = malloc(sizeof(sem_t));
 // sem_init(((mate_inner_structure *)lib_ref->group_info)->sem_instance, 0, value);
  enviar_sem_init(sem, (int)value, ((mate_inner_structure *)lib_ref->group_info)->conexion, INIT_SEMAFORO); // codear fiuncion; pasar valor
  char* respuesta = recibir_mensaje(((mate_inner_structure *)lib_ref->group_info)->conexion); // espera respuesta para continuar ejecutando condigo???

  return 0;
  }
  return -1;//PREGUBNTAR POR RETORNO DE ENTERO
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem) {
  if(((mate_inner_structure *)lib_ref->group_info)->con_kernel){
    enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_WAIT);
    return 0;
  }
   // return sem_wait(((mate_inner_structure *)lib_ref->group_info)->sem_instance);
   return -1;
 }

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem) {
  if(((mate_inner_structure *)lib_ref->group_info)->con_kernel){
  enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_POST);
  return 0;
}
return -1;
}
int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem) {
  if(((mate_inner_structure *)lib_ref->group_info)->con_kernel){
  enviar_mensaje_y_cod_op(sem, ((mate_inner_structure *)lib_ref->group_info)->conexion, SEM_DESTROY);
  return 0;
}
return -1;
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
  //((mate_inner_structure *)lib_ref->group_info)->memory = malloc(size);
  return 0;
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr)
{
  if (addr != 0)
  {
    return -1;
  }
  //free(((mate_inner_structure *)lib_ref->group_info)->memory);
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

