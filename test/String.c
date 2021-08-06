#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Object.c"

typedef struct {
Object_t *__extends;
char* text;
long length;
long allocatedLength;
void(*append) (char* extra) ;
char*(*charAt) (long index) ;

} String_t;

String_t* String_constructor() {
String_t* __String__obj = malloc(sizeof(String_t));

__String__obj->__extends = Object_constructor();
__String__obj->str = malloc(10 * sizeof(char));
__String__obj->length = 0;
__String__obj->allocatedLength = 10;
__String__obj->append=&String_append;
__String__obj->charAt=&String_charAt;

return __String__obj;
}
void String_append(char* extra) {
strcat(text, extra);
}
char* String_charAt(long index) {
return text[index];
}
