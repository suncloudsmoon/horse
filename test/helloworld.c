#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef struct {
Object *__extends;
unsafe_char* name;

} helloworld;

helloworld* helloworld_constructor(char*args){
printf(args);
}
