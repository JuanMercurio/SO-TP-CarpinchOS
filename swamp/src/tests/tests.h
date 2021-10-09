#ifndef _TESTS_SWAMP_
#define _TESTS_SWAMP_

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

void suma();

#endif