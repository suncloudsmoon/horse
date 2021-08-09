#ifdef Compiler_H_
#define Compiler_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IO__CLASS.h"

typedef struct Compiler_t Compiler_t;
struct Compiler_t{
void(*main) (char *args) ;

};
void main(char *args) ;

#endif
