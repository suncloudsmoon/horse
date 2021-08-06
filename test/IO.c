#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "String.c"

typedef struct {
Object_t *__extends;
long(*print) (char* text, ...) ;

} IO_t;

IO_t* IO_constructor(){
IO_t* __IO__obj = malloc(sizeof(IO_t));

__IO__obj->__extends = Object_constructor();
__IO__obj->print=&IO_print;

return __IO__obj;
}
long IO_print(char* text, ...) {
__IO__obj->va_list args;
__IO__obj->va_start(args, text);
__IO__obj->vfprintf(stdout, text, args);
__IO__obj->va_end(args);
}
