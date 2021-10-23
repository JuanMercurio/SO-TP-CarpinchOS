#include "tests.h"
#include "../main.h"

#include "../configuracion/config.h"
#include <CUnit/Basic.h>
#include <stdlib.h>

#include "../esquema/tlb.h"

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