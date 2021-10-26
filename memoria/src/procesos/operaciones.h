#ifndef _OPERACIONES_MEMORIA_
#define _OPERACIONES_MEMORIA_

#include <stdint.h>
#include <commons/collections/list.h>

/* Estrucutra de los alloc */
typedef struct HeadMetaData{ 
    uint32_t prevAlloc;
    uint32_t nextAlloc;
    uint8_t isFree;
}HeadpMetaData;

#endif