#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
long hashCode;

} Object_t;

Object_t* Object_constructor(){
Object_t* __Object__obj = malloc(sizeof(Object_t));

__Object__obj->hashCode = 1;

return __Object__obj;
}
