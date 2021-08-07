#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Object.c"

typedef struct {
Object_t *__extends;
void(*append) ,char* extra) char* extra) ;
char*(*charAt) ,long index) long index) ;

} String_t;

char* text;
long length;
long allocatedLength;
void String_append,char* extra) char* extra) {
strcat(text, extra);
}
char* String_charAt,long index) long index) {
return text[index];
}

String_t* String_constructor() {
String_t* __String__obj = malloc(sizeof(String_t));

__String__obj->__extends = Object_constructor();
str = malloc(_String__obj,10 * sizeof(char);
length = 0;
allocatedLength = 10;
__String__obj->append=&String_append;
__String__obj->charAt=&String_charAt;
return __String__obj;
}
