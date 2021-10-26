#ifndef _TESTS_MEMORIA_
#define _TESTS_MEMORIA_

/*
    @NAME: run_tests
    @DESC: Funcion principal que corre los tests
 */
int run_tests();


/*
    @NAME: correr_tests
    @DESC: Se encarga de ver si el usuario quiere hacer tests o correr programa normalmente
    @PARA: arg  - cantidad de paramentros que inserto el usuario
           argv - el primer parametros que inserto el usuario  
 */
void tests(int arg, char* argv);

/*
    @NAME: test_inicio_tlb
    @DESC: Test para ver si la tlb al iniciarse lo hace correctamente
 */
void test_inicio_tlb();

void suma();
void test_mensaje_serializado();
void test_tlb_miss();
void test_decimal_a_binario();
void test_converir_a_df();
void test_crear_dl();
void test_traducir_dir_log();
void test_get_offset();
void test_traducciones();
void test_no_frames_disponibles();

#endif
