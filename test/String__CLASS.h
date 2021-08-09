#ifdef String_H_
#define String_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Object__CLASS.h"

typedef struct String_t String_t;
struct String_t{
Object_t *__extends;
char* text;
long length;
long allocatedLength;
void(*append) (String_t* __String__obj,char* extra) ;
char(*charAt) (String_t* __String__obj,long index) ;

};
void String_append(String_t* __String__obj,char* extra) ;
char String_charAt(String_t* __String__obj,long index) ;

#endif
