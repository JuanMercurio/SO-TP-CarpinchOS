#include "utils.h"

int crearID(int *n){
    return __atomic_fetch_add(n, 1, __ATOMIC_SEQ_CST);
}

int suma_atomica(int *n){
    return crearID(n);
}

