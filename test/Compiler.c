#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IO.c"

typedef struct Compiler_t Compiler_t;
struct Compiler_t{
void(*main) (char *args) ;

}
void main(char *args) {
 output=IO_constructor();

}

Compiler_t* Compiler_constructor(char *args){
Compiler_t* __Compiler__obj = malloc(sizeof(Compiler_t));
__Compiler__obj->main=&main;
return __Compiler__obj;
}
