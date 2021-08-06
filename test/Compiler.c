#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IO.c"

typedef struct {
void(*main) () ;
Object*(*startSomewhere) () ;

} Compiler_t;

Compiler_t* Compiler_constructor(char *args){
Compiler_t* __Compiler__obj = malloc(sizeof(Compiler_t));

__Compiler__obj->main=&main;
__Compiler__obj->startSomewhere=&Compiler_startSomewhere;

return __Compiler__obj;
}
void main() {
__Compiler__obj->IO output = new IO();
__Compiler__obj->output->print("Hello World!!!!!!");
}
Object* Compiler_startSomewhere() {
__Compiler__obj->printf();
}
