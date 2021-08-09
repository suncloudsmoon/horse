#ifdef IO____CLASS_H_
#define IO____CLASS_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "String__CLASS.h"

typedef struct IO_t IO_t;
struct IO_t{
Object_t *__extends;
long(*print) (IO_t* __IO__obj,char* text) ;

};
long IO_print(IO_t* __IO__obj,char* text) ;

#endif
