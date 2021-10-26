#include "tests.h"
#include "../main.h"
#include "../esquema/tlb.h"
#include "../configuracion/config.h"
#include "../esquema/paginacion.h"

#include <CUnit/Basic.h>
#include <stdlib.h>
#include <conexiones/conexiones.h>


void tests(int arg, char* argv){
    if(arg > 1 && strcmp(argv, "test")==0){
        int a = run_tests();
        exit(a);
    }
}

int run_tests(){

    CU_initialize_registry();
   
    CU_pSuite tests = CU_add_suite("Suite",NULL,NULL);
    CU_add_test(tests,"Probar Suma", suma);
    CU_add_test(tests, "Probar Inicio TLB", test_inicio_tlb);
    CU_add_test(tests, "Probar TLB MISS", test_tlb_miss);
    CU_add_test(tests, "Probar Serializacion de Mensajes", test_mensaje_serializado);
    CU_add_test(tests, "Probar decilmal a binario", test_decimal_a_binario);
    CU_add_test(tests, "Probar convertir a df", test_converir_a_df);
    CU_add_test(tests, "Probar creacion dl", test_crear_dl);
    CU_add_test(tests, "Probar get offset de una dl", test_get_offset);
    CU_add_test(tests, "Probar traduccion dl", test_traducir_dir_log);
    CU_add_test(tests, "Probar trauccion ida y vuelta", test_traducciones);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    
    return CU_get_error();
}

void suma(){
    CU_ASSERT_EQUAL(2+2, 4);
}

void test_inicio_tlb(){
    obtener_config();
    iniciar_tlb();
    tlb_t* registro = list_get(tlb, configuracion.CANTIDAD_ENTRADAS_TLB -1);
    CU_ASSERT_EQUAL(registro->pid, EMPTY);
    CU_ASSERT_EQUAL(registro->pagina, EMPTY);
    CU_ASSERT_EQUAL(registro->marco, EMPTY);
}

#define MISSING_PID -100
#define MISSING_PAGE -100
void test_tlb_miss(){
   iniciar_tlb(); 
   CU_ASSERT_EQUAL(buscar_en_tlb(MISSING_PID, MISSING_PAGE), TLB_MISS);   
}
 
void test_mensaje_serializado(){
    void* buffer = serializar_mensaje("PRUEBA");
    int size;
    memcpy(&size, buffer, sizeof(int));
    int offset = sizeof(int);

    char* final = malloc(size);
    memcpy(final, buffer + offset, size);
    CU_ASSERT_STRING_EQUAL(final, "PRUEBA");
}

void test_decimal_a_binario(){
   int binario = decimal_a_binario(16);
   CU_ASSERT_EQUAL(binario, 10000);
}

void test_converir_a_df(){
    t_list* tabla = list_create();
    pag_t* reg = malloc(sizeof(pag_t));
    reg->algoritmo = 0; 
    reg->marco = 10;
    reg->modificado = 0;
    reg->presente = 1;
    list_add(tabla, reg);
    dir_t dl;
    dl.offset = 100;
    dl.PAGINA = 0;
    dir_t df = convertir_a_df(tabla, dl);
    CU_ASSERT_EQUAL(df.MARCO, 10);
    CU_ASSERT_EQUAL(df.offset, 100);
}


void test_crear_dl(){
    configuracion.TAMANIO_PAGINAS = 32;
    dir_t dir_l;
    dir_l.offset = 1;
    dir_l.PAGINA = 1;
    uint32_t dl = crear_dl(dir_l);
    CU_ASSERT_EQUAL(dl, 100001);
}

void test_traducir_dir_log(){
    configuracion.TAMANIO_PAGINAS = 32;
    dir_t dir_l = traducir_dir_log(1100110);
    CU_ASSERT_EQUAL(dir_l.offset, 6);
    CU_ASSERT_EQUAL(dir_l.PAGINA, 3);
}

void test_get_offset(){
    int offset = get_offset(1110, 1000);
    CU_ASSERT_EQUAL(offset, 110);

}

void test_traducciones(){
    configuracion.TAMANIO_PAGINAS = 32;
    dir_t dir_l = traducir_dir_log(10001);
    int dl = crear_dl(dir_l);
    CU_ASSERT_EQUAL(dl, 10001)
}