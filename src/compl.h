// This file was automatically generated by the horse compiler
#ifndef compl_H_
#define compl_H_
#include "__DEF__.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "__DEF__.h"
#include "errno.h"
#include "stdarg.h"
#include "ctype.h"
#include "string.h"
#include "time.h"

#define AVG_STRING_SIZE 32
#define STRING_ALLOCATION_SIZE 5
#define LIST_MANAGER_ALLOC_SIZE 10
typedef enum {
ERRNO_EXCEPTION = 1,
NULL_POINTER_EXCEPTION = 2,
INDEX_OUT_OF_BOUNDS_EXCEPTION = 3
} exception;
typedef struct {
char *text;
size_t text_length;
size_t text_allocated_length;
} string_t;
typedef struct {
void **data;
size_t data_length;
size_t data_allocated_length;
} list_t;
typedef struct {
list_t *allLines; // List of all lines
list_t *cleanedLines;
list_t *parsedLines; // List of parsed lines
list_t *compiledLines;
list_t *definitions;
string_t *includeStatements;
list_t *classes; // list of class_t
string_t* directory;
FILE *inputFile;
num scope;
num currentClass; // index of the current class found in the list of classes
} compiler_t;
typedef struct {
string_t *name;
list_t *constructorLines;
list_t *globalVariables; // like int a;
list_t *vTableList; // list of vTable_t
string_t *includeStatements;
list_t *definitions; // and definitions
list_t *prototypes; // function prototypes
list_t *restOfLines;
} class_t;
typedef struct {
string_t *returnType;
string_t *name;
list_t *parameters;
} vTable_t;
const char *doubleSlashComment = "//";
const char *functionIdentifier = "def";
const char *privateFunctionIdentifier = "private def";
const char *endIdentifier = "end";
const char *returnIdentifier = "return";
const char *forIdentifier = "for";
const char *toKeyword = "to";
const char *doKeyword = "do";
const char *thenKeyword = "then";
const char *ifIdentifier = "if"; // for now
const char *elseIfIdentifier = "or if";
const char *elseIdentifier = "or";
const char *whileIdentifier = "while";
const char *classIdentifier = "class";
const char *takeIdentifier = "take"; // like switch()
const char *importIdentifier = "import"; // add basics to imports
const char *numIdentifier = "num"; // num science = 5
const char *numDataType = "long long int";
const char *defFileName = "DEF";
/*
returns -1 if no stopSign was found
*/
typedef struct compl compl;
struct compl {
 int (*main) (int argc, char **argv);
 void (*startCompiler) (string_t *directory, string_t *inputFilename);
 compiler_t* (*compiler_init) (string_t* directory, FILE *inputFile);
 void (*compiler_free) (compiler_t *com);
 class_t* (*class_new) (string_t *name, string_t *includeStatements);
 void (*vTable_new) (string_t *returnType, string_t *name, list_t *parameters, vTable_t *vTable);
 void (*vTable_delete) (vTable_t *vTable);
 void (*ignition) (compiler_t *com);
 void (*readAllLines) (compiler_t *com);
 void (*parse) (compiler_t *com);
 void (*compile) (compiler_t *com);
 void (*writeToFile) (compiler_t *com);
 list_t* (*list_init) ();
 void (*list_add) (void *item, list_t *list);
 void (*list_remove) (int index, list_t *list);
 void (*list_complete_remove) (void (*indivfree)(void*), int index, list_t *list);
 void (*list_clear) (list_t *list);
 bool (*list_equals) (void *destComp, int index, bool (*equalsComparator)(void*, void*), list_t *list);
 bool (*list_contains) (void *destComp, bool (*equalsComparator)(void*, void*), list_t *list);
 void (*list_serialize) (void (*indiv)(void*, FILE*), FILE *stream, list_t *list);
 list_t* (*list_deserialize) (void* (*indivreverse)(FILE*), FILE *stream);
 void (*list_free) (list_t *list);
 void (*list_complete_free) (void (*indivfree)(void*), list_t *list);
 string_t* (*string_init) ();
 string_t* (*string_copyvalueof) (char *src);
 string_t* (*string_copyvalueof_s) (string_t *src);
 void (*string_printf) (string_t *dest, char *format, ...);
 void (*string_append) (string_t *dest, char *src);
 void (*string_append_s) (string_t *dest, string_t *src);
 void (*string_appendchar) (string_t *dest, char letter);
 bool (*string_isempty) (string_t *str);
 int (*string_indexof_s) (string_t *src, char *stopSign);
 int (*string_lastindexof_s) (string_t *src, char *stopSign);
 string_t** (*string_split) (char delimiter, string_t *src);
 bool (*string_equals) (string_t *dest, const char *src);
 bool (*string_equals_s) (string_t *dest, string_t *src);
 bool (*string_equalsignorecase) (string_t *dest, const char *src);
 bool (*string_equalsignorecase_s) (string_t *dest, string_t *src);
 bool (*string_startswith_s) (string_t *src, string_t *search);
 bool (*string_startswith) (string_t *src, const char *search);
 string_t* (*string_substring_s) (int startIndex, int endIndex, string_t *src);
 void (*string_tolowercase_s) (string_t *dest);
 bool (*string_serialize) (string_t *src, FILE *stream);
 string_t* (*string_deserialize) (FILE *stream);
 void (*string_reset) (string_t *dest);
 void (*string_free) (void *dest);
 void (*throw_exception) (exception e, int lineNum, char *message, ...);
};

#endif
