#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Object_t Object_t;
struct Object_t{
long hashCode;

}

Object_t* Object_constructor(){
Object_t* __Object__obj = malloc(sizeof(Object_t));
__Object__obj->hashCode;
return __Object__obj;
}
