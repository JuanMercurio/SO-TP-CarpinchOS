#include "utils.h"

int crearID(int *n){
    return __atomic_fetch_add(n, 1, __ATOMIC_SEQ_CST);
}

int suma_atomica(int *n){
    return crearID(n);
}

int min_get(int a, int b)
{
    return a < b ? a : b;
}

int max_get(int a, int b)
{
    return a > b ? a : b;
}

