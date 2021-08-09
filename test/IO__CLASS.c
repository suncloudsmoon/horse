#include "IO__CLASS.h"
IO_t* IO_constructor(){
IO_t* __IO__obj = malloc(sizeof(IO_t));
__IO__obj->__extends = Object_constructor();
__IO__obj->print=&IO_print;
return __IO__obj;
}

long IO_print(IO_t* __IO__obj,char* text) {

}
