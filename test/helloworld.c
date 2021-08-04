#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
Object*(*startSomewhere) () ;

} helloworld_t;

helloworld_t* helloworld_constructor(char *args){
helloworld_t* __helloworld__obj = malloc(sizeof(helloworld_t));
__helloworld__obj->startSomewhere=&helloworld_startSomewhere;

fprintf(stdout, "Hello World!\n");
fprintf(stdout, "Hello World!\n");
return __helloworld__obj;
}
Object* startSomewhere() {
}
