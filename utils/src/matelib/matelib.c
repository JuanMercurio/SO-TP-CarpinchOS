
#include "matelib.h"

//------------------General Functions---------------------/

int mate_init(mate_instance *lib_ref, char *config)
{
  t_config *configuracion = config_create(config);
  char* IP = config_get_string_value(configuracion, "IP");
  char* PUERTO = config_get_string_value(configuracion, "PUERTO");
  

  lib_ref->group_info = malloc(sizeof(mate_inner_structure));
  mate_inner_structure mate_ref;
  lib_ref->group_info = &mate_ref;
  mate_ref.pid = generear_pid();

  lib_ref->conexion = crear_conexion(IP, PUERTO);
  enviar_instancia(lib_ref); // serializar structura 
  char * respuesta = recibir_mensaje(lib_ref->conexion);// esta respuesta la debe esperar por si no hay memoria y nivel de multi programacion????
                                                  // una vez registrada en kernel creando sus estructuras, se le da permiso de ejecutar 
                                                  // despues de ser planificada, por ende queda bloqueado aca hasta que el kernel la pase a listo?

  return 0;
}

int mate_close(mate_instance *lib_ref)
{
  free(lib_ref->group_info);
  return 0;
}

//-----------------Semaphore Functions---------------------/

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value) {
  
  ((mate_inner_structure *)lib_ref->group_info)->sem_instance = malloc(sizeof(sem_t));
  sem_init(((mate_inner_structure *)lib_ref->group_info)->sem_instance, 0, value);
  enviar_mensaje(sem, lib_ref->conexion, INIT_SEMAFORO); // codear fiuncion;
  char* respuesta = recibir_mensaje(lib_ref->conexion); // espera respuesta para continuar ejecutando condigo???

  return 0;
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem) {
  if (strncmp(sem, "SEM1", 4)) // para que esta esto? con que conpara?
  {
    return -1;
  }
  return sem_wait(((mate_inner_structure *)lib_ref->group_info)->sem_instance);
 
}

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem) {
  if (strncmp(sem, "SEM1", 4))
  {
    return -1;
  }
  return sem_post(((mate_inner_structure *)lib_ref->group_info)->sem_instance);
}

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem) {
  if (strncmp(sem, "SEM1", 4))
  {
    return -1;
  }
  int res = sem_destroy(((mate_inner_structure *)lib_ref->group_info)->sem_instance);
  free(((mate_inner_structure *)lib_ref->group_info)->sem_instance);
  return res;
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
  ((mate_inner_structure *)lib_ref->group_info)->memory = malloc(size);
  return 0;
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr)
{
  if (addr != 0)
  {
    return -1;
  }
  free(((mate_inner_structure *)lib_ref->group_info)->memory);
  return 0;
}

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size)
{
  if (origin != 0)
  {
    return -1;
  }
  memcpy(dest, ((mate_inner_structure *)lib_ref->group_info)->memory, size);
  return 0;
}

int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size)
{
  if (dest != 0)
  {
    return -1;
  }
  memcpy(((mate_inner_structure *)lib_ref->group_info)->memory, origin, size);
  return 0;
}

//--------------------Auxiliar Functions-----------------//

uint32_t generar_pid(){ // bucar manera de generar pids sin que se repitan y sin hacer uso de variable globales
  return 1;
}
/* 
//-------------------Messages Functions------------------//
int crear_conexion(char *ip, char* puerto)		//puede ser optimizada?
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("No se pudo conectar al Servidor\n");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente, int codigo_op){//usada para enviar tarea desde mi ram el codigo de operacion sera indistinto
	printf("preparando mensjae\n");
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_op;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);
	printf("mensaje enviado\n");
	free(a_enviar);
	eliminar_paquete(paquete);
}

char* recibir_mensaje(int socket_cliente)
{
	int size;

	char* buffer = recibir_buffer(&size, socket_cliente);

	printf("recibir_mensaje: Mensaje recibido %s\n", buffer);
	//log_info(logger, "Me llego el mensaje %s", buffer);
	return buffer;
}
*/