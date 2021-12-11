#include <matelib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
//#include <lib/matelib.h>
#include <commons/log.h>
#include <semaphore.h>

typedef struct thread_info
{
  char* mate_cfg_path;
  uint32_t th_number;
  sem_t* producer_sem;
  sem_t* consumer_sem;
} thread_info;

char *LOG_PATH = "./cfg/carpincho.log";
char *PROGRAM_NAME = "carpincho";
uint32_t seed;
sem_t seed_sem_1;
sem_t seed_sem_2;
t_log *logger;
pthread_mutex_t logger_mutex;

void print_thread_info(char *thread_name, uint32_t value)
{
  pthread_mutex_lock(&logger_mutex);
  log_info(logger, "thread name: %s", thread_name);
  log_info(logger, "value: %d", value);
  pthread_mutex_unlock(&logger_mutex);
}

void log_message(char *message, va_list args) {
  pthread_mutex_lock(&logger_mutex);
  log_info(logger, message, args);
  pthread_mutex_unlock(&logger_mutex);
}

void calculate_value_and_increment_seed(uint32_t *current_value)
{
  (*current_value) = seed;
  seed++;
}

void *carpincho_acaparador(void * config) {
    printf("paso 1.1\n");
  thread_info *info = (thread_info *) config;

  char *thread_name = malloc(10);
  sprintf(thread_name, "%s%d", "CARPINCHO", info->th_number);
    printf("paso 1.2\n");
  mate_instance mate_ref;
  mate_init(&mate_ref, info->mate_cfg_path);
    printf("paso 1.3\n");
  mate_pointer key = mate_memalloc(&mate_ref, 10);
  mate_memwrite(&mate_ref, thread_name, key, 10);
    printf("paso 1.4\n");
  //acaparo un archivo de swamp
  mate_memalloc(&mate_ref, 20000);
    printf("paso 1.5\n");
  while(1) {
    mate_memread(&mate_ref, key, thread_name, 10);
    log_message("thread name: %s", thread_name);
  }
  printf("paso 1.6\n");
}

void *carpincho(void *config)
{
    printf("paso 2.0\n");
  thread_info *info = (thread_info *) config;
printf("paso 2.1\n");
  char *thread_name = malloc(10);
  sprintf(thread_name, "%s%d", "CARPINCHO", info->th_number);
printf("paso 2.2\n");
  mate_instance mate_ref;
  mate_init(&mate_ref, info->mate_cfg_path);
printf("paso 2.3\n");
  mate_pointer key = mate_memalloc(&mate_ref, 10);
  mate_memwrite(&mate_ref, thread_name, key, 10);
printf("paso 2.4\n");
  mate_pointer value = mate_memalloc(&mate_ref, sizeof(uint32_t));
  mate_memwrite(&mate_ref, &seed, value, sizeof(uint32_t));
  print_thread_info(thread_name, seed);
    printf("paso 2.5\n");
  uint32_t current_value;
  
  int vuelta = 0;
  while (vuelta < 100)
  {
      printf("paso 2.6\n");
    sem_wait(info->producer_sem);
    mate_memread(&mate_ref, key, thread_name, 10);
    mate_memread(&mate_ref, value, &current_value, sizeof(uint32_t));

    // Pido memoria sin guardar el puntero. No creo que sea un problema
    mate_memalloc(&mate_ref, 1);
    printf("paso 2.61\n");
    calculate_value_and_increment_seed(&current_value);
    print_thread_info(thread_name, current_value);
    printf("paso 2.62\n");
    mate_memwrite(&mate_ref, &current_value, value, sizeof(uint32_t));
    sem_post(info->consumer_sem);
    printf("paso 2.63\n");
    vuelta++;
  }
    printf("paso 2.7\n");
  free(thread_name);
  mate_close(&mate_ref);
  return 0;
}

int main(int argc, char *argv[])
{

  logger = log_create(LOG_PATH, PROGRAM_NAME, true, 2);
  /*if (argc != 2)
  {
    log_info(logger, "Debe ingresar el path del archivo de config\n");
    return -1;
  }*/
  printf("paso 1\n");
  argv = string_duplicate("/cfg/carpincho.config");

  char *config_path = "./cfg/carpincho.config";
  seed = 1;
   printf("paso 2\n");
  pthread_mutex_init(&logger_mutex, NULL);
  sem_init(&seed_sem_1, 0, 1);
  sem_init(&seed_sem_2, 0, 0);
     printf("paso 3\n");
  pthread_t carpincho_th_1;
  pthread_t carpincho_th_2;
  pthread_t carpincho_th_3;
 printf("paso 4\n");
  thread_info *th_1_info = malloc(sizeof(thread_info));
  th_1_info->mate_cfg_path = config_path;
  th_1_info->th_number = 1;
  th_1_info->consumer_sem = &seed_sem_2;
  th_1_info->producer_sem = &seed_sem_1;
 printf("paso 5\n");
  thread_info *th_2_info = malloc(sizeof(thread_info));
  th_2_info->mate_cfg_path = config_path;
  th_2_info->th_number = 2;
  th_2_info->consumer_sem = &seed_sem_1;
  th_2_info->producer_sem = &seed_sem_2;
 printf("paso 6\n");
  thread_info *th_3_info = malloc(sizeof(thread_info));
  th_3_info->mate_cfg_path = config_path;
  th_3_info->th_number = 3;
 printf("paso 7\n");
  pthread_create(&carpincho_th_3, NULL, &carpincho_acaparador, (void *)th_3_info);
  pthread_create(&carpincho_th_1, NULL, &carpincho, (void *)th_1_info);
  pthread_create(&carpincho_th_2, NULL, &carpincho, (void *)th_2_info);
   printf("paso 8\n");
  pthread_join(carpincho_th_1, NULL);
  pthread_join(carpincho_th_2, NULL);
   printf("paso 9\n");
  return 0;
}