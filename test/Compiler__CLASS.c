#include "Compiler__CLASS.h"
Compiler_t* Compiler_constructor(char *args){
Compiler_t* __Compiler__obj = malloc(sizeof(Compiler_t));
__Compiler__obj->main=&main;
return __Compiler__obj;
}

void main(char *args) {
 output=IO_constructor();

}
