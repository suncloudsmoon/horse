#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Object.c"

typedef struct String_t String_t;
struct String_t{
Object_t *__extends;
char* text;
long length;
long allocatedLength;
void(*append) (String_t* __String__obj,char* extra) ;
char(*charAt) (String_t* __String__obj,long index) ;

}
void String_append(String_t* __String__obj,char* extra) {

}
char String_charAt(String_t* __String__obj,long index) {
return text[index];
}

String_t* String_constructor() {
String_t* __String__obj = malloc(sizeof(String_t));
__String__obj->__extends = Object_constructor();
__String__obj->text=malloc(10 * sizeof(char));
__String__obj->length;
__String__obj->allocatedLength;
__String__obj->append=&String_append;
__String__obj->charAt=&String_charAt;
return __String__obj;
}
