/*
 * Copyright (c) 2021, suncloudsmoon and the horse contributors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * main.c
 *
 *  Created on: Aug 9, 2021
 *      Author: suncloudsmoon
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "__DEF__.h"
#include "errno.h"
#include "stdarg.h"
#include "ctype.h"
#include "string.h"

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
	list_t *classes; // list of class_t

	string_t* directory;
	FILE *inputFile;
	FILE *outputFile;
	
long long int  scope;
long long int  currentClass;
} compiler_t;

typedef struct {
	string_t *name;
	list_t *variables;
	list_t *restOfLines;
} class_t;

// Exception Stuff
void throw_exception(exception e, int lineNum, char *message, ...);

// String stuff
static string_t* custom_string_init(size_t allocationSize);
static void string_meminspection(size_t addNum, string_t *subject);
string_t* string_init();
string_t* string_copyvalueof(char *src);
string_t* string_copyvalueof_s(string_t *src);
void string_printf(string_t *dest, char *format, ...);
void string_append(string_t *dest, char *src);
void string_append_s(string_t *dest, string_t *src);
void string_appendchar(string_t *dest, char letter);
int string_indexof_s(string_t *src, char *stopSign);
string_t** string_split(char delimiter, string_t *src);
bool string_equals(string_t *dest, const char *src);
bool string_equals_s(string_t *dest, string_t *src);
bool string_equalsignorecase(string_t *dest, const char *src);
bool string_equalsignorecase_s(string_t *dest, string_t *src);
bool string_startswith(string_t *src, const char *search);
bool string_startswith_s(string_t *src, string_t *search);
string_t* string_substring_s(int startIndex, int endIndex, string_t *src);
void string_tolowercase_s(string_t *dest);
bool string_serialize(string_t *src, FILE *stream);
string_t* string_deserialize(FILE *stream);
void string_reset(string_t *dest);
void string_free(void *dest);

// List Stuff
static list_t* custom_list_init(size_t mallocSize);
static void list_meminspector(size_t addNum, list_t *subject);
list_t* list_init();
void list_add(void *item, list_t *list);
void list_remove(int index, list_t *list);
void list_complete_remove(void (*indivfree)(void*), int index, list_t *list);
void list_clear(list_t *list);
bool list_equals(void *destComp, int index,
bool (*equalsComparator)(void*, void*), list_t *list);
bool list_contains(void *destComp, bool (*equalsComparator)(void*, void*),
		list_t *list);
void list_serialize(void (*indiv)(void*, FILE*), FILE *stream, list_t *list);
list_t* list_deserialize(void* (*indivreverse)(FILE*), FILE *stream);
void list_free(list_t *list);
void list_complete_free(void (*indivfree)(void*), list_t *list);

// Compiler Functions
compiler_t* compiler_init(string_t *directory, FILE *inputFile, FILE *outputFile);
void compiler_free(compiler_t *com);
void ignition(compiler_t *com);
void readAllLines(compiler_t *com);
static bool readLine(FILE *stream, string_t *line);
void parse(compiler_t *com);
static list_t* split(char delimiter, string_t *line);
static bool isSpecialCharacter(char alpha);
void compile(compiler_t *com);
int writeToFile(compiler_t *com);

// Horse Identifiers
const char *functionIdentifier = "function";
const char *privateFunctionIdentifier = "private function";
const char *endIdentifier = "end";

const char *thenKeyword = "then";
const char *ifIdentifier = "if"; // for now
const char *elseIfIdentifier = "or if";
const char *elseIdentifier = "or";
const char *forIdentifier = "for";
const char *whileIdentifier = "while";
const char *classIdentifier = "class";
const char *takeIdentifier = "take"; // like switch()
const char *importIdentifier = "import"; // add basics to imports

const char *numIdentifier = "num"; // num science = 5
const char *numDataType = "long long int";

const char *defFileName = "DEF";

  int  main(int argc, char **argv)  {
	string_t *inputFilename, *outputFilename, *directory;
	// Arguments: [compiler executable] [directory] [directory/inputFile] [directory/outputFile]
if ( (argc >= 4)) {
		printf("[Debug] Successfully accessing the command line arguments!\n");
		directory = string_copyvalueof(argv[1]);
		inputFilename = string_copyvalueof(argv[2]);
		outputFilename = string_copyvalueof(argv[3]);
} else {
		directory = string_copyvalueof("src");
		inputFilename = string_copyvalueof("src/compl.hr");
		outputFilename = string_copyvalueof("src/compl.c");
}
	
	FILE *input = fopen(inputFilename->text, "r");
	FILE *output = fopen(outputFilename->text, "w");

	compiler_t *com = compiler_init(directory, input, output);
	ignition(com);
	compiler_free(com);

	return 0;
}

  compiler_t*  compiler_init(string_t* directory, FILE *inputFile, FILE *outputFile)  {
	compiler_t *com = malloc(sizeof(compiler_t));
	com->allLines = list_init();
	com->cleanedLines = list_init();
	com->parsedLines = list_init();
	com->compiledLines = list_init();

	com->definitions = list_init();
	com->classes = list_init();

	com->directory = directory;
	com->inputFile = inputFile;
	com->outputFile = outputFile;
	com->currentClass = -1; // currently in no class
	return com;
}

  void  compiler_free(compiler_t *com)  {
	list_complete_free(&string_free, com->allLines);
for (long long int  i  = 0;  i  <  (int)com->parsedLines->data_length ;  i++) {
		list_complete_free(&string_free, com->parsedLines->data[i]);
}
	list_complete_free(&string_free, com->compiledLines);
	free(com);
}

  void  ignition(compiler_t *com)  {
	printf("[ignition] program starting!\n");
	readAllLines(com);
	parse(com);
	compile(com);
	writeToFile(com);
}

  void  readAllLines(compiler_t *com)  {
	string_t *line = string_init();
while ( !readLine(com->inputFile, line) ) {
		list_add(string_copyvalueof_s(line), com->allLines);
		string_reset(line);
}
	string_free(line);
}

static  bool  readLine(FILE *stream, string_t *line)  {
	char letter;
while ( (letter = fgetc(stream)) != EOF && letter != '\n' ) {
		string_appendchar(line, letter);
}
	return letter == EOF;
}

  void  parse(compiler_t *com)  {
for (long long int  i  = 0;  i  <  (int)com->allLines->data_length ;  i++) {
		string_t *existingLine = (string_t*) com->allLines->data[i];
		int firstLetter = strcspn(existingLine->text,
				"abcdefghijklmnopqrstuvwxyz0123456789#{}/");
		string_t *initialClean = string_substring_s(firstLetter,
				existingLine->text_length, existingLine);
		list_add(initialClean, com->cleanedLines);		
		list_add(split(' ', initialClean), com->parsedLines);
}
}

static  list_t*  split(char delimiter, string_t *line)  {
	list_t *output = list_init();
	string_t *temp = string_init();
	bool isSpecial = false;
for (long long int  i  = 0;  i  <  (int)line->text_length ;  i++) {
		char alpha = line->text[i];
if ( isSpecialCharacter(alpha)) {
			isSpecial = !isSpecial;
} else if ( !isSpecial && alpha == delimiter) {
			list_add(string_copyvalueof_s(temp), output);
			string_reset(temp);
}
		string_appendchar(temp, alpha);
}
	list_add(temp, output);
	return output;
}

static  bool  isSpecialCharacter(char alpha)  {
	return alpha == '"' || alpha == '\'' || alpha == '(' || alpha == ')';
}

  class_t*  class_new(string_t *name)  {
	class_t *newClass = malloc(sizeof(class_t));
	newClass->name = string_copyvalueof_s(name);
	newClass->variables = list_init();
	newClass->restOfLines = list_init();
	return newClass;
}

  void  compile(compiler_t *com)  {
	// add definitions to file
	list_add(string_copyvalueof("#define num long long int"), com->definitions);
	string_t *parsed;
for (long long int  i  = 0;  i  <  (int)com->parsedLines->data_length ;  i++) {
		list_t *tokens = (list_t *) com->parsedLines->data[i];
		string_t *firstToken = (string_t*) tokens->data[0];
		string_t *line = (string_t*) com->cleanedLines->data[i];

		// printf("First token: %s\n", firstToken->text);
if ( string_equals(firstToken, classIdentifier)) {
			// class blah not class blah extends object
			string_t *className = string_substring_s(strlen(classIdentifier) + 1, line->text_length, line);
			list_add(class_new(className), com->classes);
			com->currentClass++;
			continue;

} else if ( string_startswith(line, functionIdentifier)) {
			// function blah() returns void
			parsed = string_init();
			string_t *function = string_substring_s(strlen(functionIdentifier), string_indexof_s(line, "returns"), line);
			string_t *returnType = string_substring_s(string_indexof_s(line, "returns") + strlen("returns"), line->text_length, line);
			string_printf(parsed, " %s %s {", returnType->text, function->text);
			com->scope++;

} else if ( string_startswith(line, privateFunctionIdentifier)) {
			// private function blah() returns void
			parsed = string_init();
			string_t *function = string_substring_s(strlen(privateFunctionIdentifier), string_indexof_s(line, "returns"), line);
			string_t *returnType = string_substring_s(string_indexof_s(line, "returns") + strlen("returns"), line->text_length, line);
			string_printf(parsed, "static %s %s {", returnType->text, function->text);

} else if ( string_startswith(line, endIdentifier)) {
			parsed = string_copyvalueof("}");
			com->scope--;

} else if ( string_startswith(line, ifIdentifier)) {
			// if blah then
			parsed = string_init();
			string_t *middle = string_substring_s(strlen(ifIdentifier), line->text_length - strlen(thenKeyword) - 2, line);
			string_printf(parsed, "if (%s) {", middle->text);
			com->scope++;

} else if ( string_startswith(line, elseIfIdentifier)) {
			parsed = string_init();
			string_t *middle = string_substring_s(strlen(elseIfIdentifier), line->text_length - strlen(thenKeyword) - 2, line);
			string_printf(parsed, "} else if (%s) {", middle->text);
			com->scope++;
		
} else if ( string_startswith(line, elseIdentifier)) {
			parsed = string_init();
			string_append(parsed, "} else {");
			com->scope++;

} else if ( string_startswith(line, forIdentifier)) {
			// for i to 5 do i++
			parsed = string_init();
			string_t *i = string_substring_s(strlen(forIdentifier), string_indexof_s(line, "to"), line);
			string_t *condition = string_substring_s(string_indexof_s(line, "to") + strlen("to"), string_indexof_s(line, "do"), line);
			string_t *increment = string_substring_s(string_indexof_s(line, "do") + strlen("do"), line->text_length, line);
			string_printf(parsed, "for (long long int %s = 0; %s < %s; %s) {", i->text, i->text, condition->text, increment->text);
			com->scope++;
		
} else if ( string_startswith(line, whileIdentifier)) {
			// while blah do
			parsed = string_init();
			string_t *condition = string_substring_s(strlen(whileIdentifier), string_indexof_s(line, "do"), line);
			string_printf(parsed, "while (%s) {", condition->text);
			com->scope++;

} else if ( string_startswith(line, importIdentifier)) {
			// add basics to imports
			parsed = string_init();
			string_t *importItem = string_substring_s(strlen(importIdentifier) + 1, line->text_length, line);

if ( string_startswith(importItem, "basics")) {
				string_append(parsed, "#include <stdio.h>\n#include <stdlib.h>\n#include <stdbool.h>\n");
				string_printf(parsed, "#include \"__%s__.h\"", defFileName);
} else {
				string_printf(parsed, "#include \"%s.h\"", importItem->text);

			printf("Parsed Import Item: %s\n", parsed->text);	
}

} else if ( string_equals(firstToken, numIdentifier)) {
			// num something = 5
			parsed = string_init();
			char* variable = ((string_t *) tokens->data[1])->text;
if ( string_indexof_s(line, "=") != -1) {
				string_printf(parsed, "%s %s = %s;", numDataType, variable, ((string_t *) tokens->data[3])->text);
} else {
				string_printf(parsed, "%s %s;", numDataType, variable);
}
} else {
			parsed = string_copyvalueof_s((string_t*) com->allLines->data[i]);
}
		list_add(parsed, com->compiledLines);
}
}

  int  writeToFile(compiler_t *com)  {
	FILE *output = com->outputFile;
	string_t *fullDefPath = string_init();

	// Double underscore is a used as a way to indicate that the file is Horse language specific	
	string_printf(fullDefPath, "%s/__%s__.h", com->directory->text, defFileName);
	FILE *definitions = fopen(fullDefPath->text, "w");
	fprintf(definitions, "#ifndef %s_H_\n", defFileName);
	fprintf(definitions, "#define %s_H_\n", defFileName);

	// Writing defintions to a separate file and the user must include it in their project
for (long long int  i  = 0;  i  <  com->definitions->data_length ;  i++) {
		fprintf(definitions, "%s\n", ((string_t *) com->definitions->data[i])->text);
}
	fprintf(definitions, "#endif\n");	
	// Free memory
	string_free(fullDefPath);
	fclose(definitions);

for (long long int  i  = 0;  i  <  (int) com->compiledLines->data_length ;  i++) {
		fprintf(output, "%s\n", ((string_t*) com->compiledLines->data[i])->text);		
}
	// Debug
	printf("[writeToFile] Successfully wrote to File!");

	return fclose(output);
}

////// LIBRARIES //////

  list_t*  list_init()  {
	list_t *list = malloc(sizeof(list_t));
	list->data = (void**) malloc(LIST_MANAGER_ALLOC_SIZE * sizeof(void*));

	list->data_length = 0;
	list->data_allocated_length = LIST_MANAGER_ALLOC_SIZE;

	return list;
}

static  list_t*  custom_list_init(size_t mallocSize)  {
	list_t *list = malloc(sizeof(list_t));
	list->data = (void**) malloc(mallocSize * sizeof(void*));

	list->data_length = 0;
	list->data_allocated_length = mallocSize;

	return list;
}

  void  list_add(void *item, list_t *list)  {
	list_meminspector(1, list);
	list->data[list->data_length] = item;
	list->data_length++;
}

  void  list_remove(int index, list_t *list)  {
	unsigned bytes = sizeof(void*) * (list->data_allocated_length - index - 1);
	memmove(&list->data[index], &list->data[index+1], bytes);
	list->data_length--;
}

  void  list_complete_remove(void (*indivfree)(void*), int index, list_t *list)  {
	(*indivfree)(list->data[index]); // frees it from the respective free method for the unknown type data
	list_remove(index, list);
}

  void  list_clear(list_t *list)  {
	list->data_length = 0;
}

  bool  list_equals(void *destComp, int index, bool (*equalsComparator)(void*, void*), list_t *list)  {
if ( index < 0 || index >= (int)list->data_length) {
		throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
				"Tried to access a list in index %d that was out of bounds!",
				index);
}

	return (*equalsComparator)(destComp, list->data[index]);
}

  bool  list_contains(void *destComp, bool (*equalsComparator)(void*, void*), list_t *list)  {
for (long long int  i  = 0;  i  <  (int) list->data_length ;  i++) {
if ( (*equalsComparator)(destComp, list->data[i])) {
			return true;
}
}
	return false;
}

  void  list_serialize(void (*indiv)(void*, FILE*), FILE *stream, list_t *list)  {
	fwrite(&list->data_length, sizeof(list->data_length), 1, stream);
for (long long int  i  = 0;  i  <  (int)list->data_length ;  i++) {
		(*indiv)(list->data[i], stream);
}
}

  list_t*  list_deserialize(void* (*indivreverse)(FILE*), FILE *stream)  {
	int arrayLength;
	fread(&arrayLength, sizeof(int), 1, stream);

	list_t *list = custom_list_init(arrayLength);
for (long long int  i  = 0;  i  <  arrayLength ;  i++) {
		list_add((*indivreverse)(stream), list);
}
	return list;
}

  void  list_free(list_t *list)  {
	free(list->data);
	free(list);
}

  void  list_complete_free(void (*indivfree)(void*), list_t *list)  {
for (long long int  i  = 0;  i  <  (int)list->data_length ;  i++) {
		(*indivfree)(list->data[i]);
}
	list_free(list);
}

static  void  list_meminspector(size_t addNum, list_t *subject)  {
if ( subject->data_length + addNum >= subject->data_allocated_length) {
		size_t newSize = 1.5 * subject->data_allocated_length + addNum;
		void **new_ptr = (void**) realloc(subject->data,
				newSize * sizeof(void*));
if ( new_ptr == NULL) {
			throw_exception(NULL_POINTER_EXCEPTION, -1,
					"Unable to allocate memory for list with length %d!",
					subject->data_length);
}

		subject->data = new_ptr;
		subject->data_allocated_length = newSize;
}
}

  string_t*  string_init()  {
	string_t *str = malloc(sizeof(string_t));

	str->text = malloc(STRING_ALLOCATION_SIZE * sizeof(char));
	str->text[0] = '\0';

	str->text_length = 0;
	str->text_allocated_length = STRING_ALLOCATION_SIZE;

	return str;
}

static  string_t*  custom_string_init(size_t allocationSize)  {
	string_t *str = malloc(sizeof(string_t));

	str->text = malloc(allocationSize);
	str->text[0] = '\0';

	str->text_length = 0;
	str->text_allocated_length = allocationSize;

	return str;
}

  string_t*  string_copyvalueof(char *src)  {
long long int  srcLength =  strlen(src);;

	string_t *dest = malloc(sizeof(string_t));
	dest->text = strdup(src);
	dest->text_length = srcLength;
	dest->text_allocated_length = srcLength + 1;

	return dest;
}

  string_t*  string_copyvalueof_s(string_t *src)  {
	string_t *dest = malloc(sizeof(string_t));
	dest->text = strdup(src->text);
	dest->text_length = src->text_length;
	dest->text_allocated_length = src->text_length + 1;

	return dest;
}

  void  string_printf(string_t *dest, char *format, ...)  {
	va_list args;
	va_start(args, format);
	int argsLength = strlen(format);
for (long long int  i  = 0;  i  <  argsLength ;  i++) {
		char first = format[i];
		char second = (i + 1 < argsLength) ? format[i+1] : '\0';
		char *arg;
if ( first == '%' && second == 's') {
			arg = va_arg(args, char*);
			i++;
} else {
			arg = malloc(2 * sizeof(char));
			arg[0] = first;
			arg[1] = '\0';
}
		int argLength = strlen(arg);
		string_meminspection(argLength, dest);
		strncat(dest->text, arg, argLength);
		dest->text_length += argLength;
}
	va_end(args);
}

  void  string_append(string_t *dest, char *src)  {
	int srcLength = strlen(src);
	string_meminspection(srcLength, dest);

	strncat(dest->text, src, srcLength);
	dest->text_length += srcLength;
}

  void  string_append_s(string_t *dest, string_t *src)  {
	string_meminspection(src->text_length, dest);
	strncat(dest->text, src->text, src->text_length);
	dest->text_length += src->text_length;
}

  void  string_appendchar(string_t *dest, char letter)  {
	string_meminspection(1, dest);
	char text[2];
	text[0] = letter;
	text[1] = '\0';
	strcat(dest->text, text);
	dest->text_length++;
}

/*
	returns -1 if no stopSign was found
*/
  int  string_indexof_s(string_t *src, char *stopSign)  {
	int stopSignLength = strlen(stopSign);
	bool found = true;
for (long long int  i  = 0;  i  <  (int) src->text_length - stopSignLength ;  i++) {
for (long long int  j  = 0;  j  <  stopSignLength ;  j++) {
if ( src->text[i+j] != stopSign[j]) {
				found = false;
				break;
}
}
if ( found) {
			return i;
}
		found = true;
}
	return -1;
}

  string_t**  string_split(char delimiter, string_t *src)  {
	// Safety
	// If the string is length 2 or less, then it is not possible to split the string
if ( src->text_length <= 2) {
		throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
				"Unable to substring a string with %d length!",
				src->text_length);
}

	char delimiterText[2];
	delimiterText[0] = delimiter;
	delimiterText[1] = '\0';
	int splitIndex = strcspn(src->text, delimiterText);

	// Was splitIndex == src->text
if ( splitIndex == (int)src->text_length) {
		return NULL;
}

	string_t **strList = malloc(2 * sizeof(string_t));
	strList[0] = custom_string_init(src->text_length / 2);
	strList[1] = custom_string_init(src->text_length / 2);

for (long long int  i  = 0;  i  <  splitIndex ;  i++) {
		string_appendchar(strList[0], src->text[i]);
}
	// for splitIndex + 1 to src->text_length do i++
for (long long int  i  = 0;  i  <  (int) src->text_length ;  i++) {
		string_appendchar(strList[1], src->text[(splitIndex + 1) + i]);
}

	return strList;
}

  bool  string_equals(string_t *dest, const char *src)  {
	unsigned srcLength = strlen(src);
if ( dest->text_length != srcLength) {
		return false;
} else {
		return strncmp(dest->text, src, srcLength) == 0;
}
}

  bool  string_equals_s(string_t *dest, string_t *src)  {
if ( dest->text_length != src->text_length) {
		return false;
} else {
		return strncmp(dest->text, src->text, src->text_length) == 0;
}
}

  bool  string_equalsignorecase(string_t *dest, const char *src)  {
if ( dest->text_length != strlen(src)) {
		return false;
} else {
for (long long int  i  = 0;  i  <  (int)dest->text_length ;  i++) {
if ( tolower(dest->text[i]) != tolower(src[i])) {
				return false;
}
}
		return true;
}
}

  bool  string_equalsignorecase_s(string_t *dest, string_t *src)  {
if ( dest->text_length != src->text_length) {
		return false;
} else {
for (long long int  i  = 0;  i  <  (int)dest->text_length ;  i++) {
if ( (tolower(dest->text[i]) != tolower(src->text[i]))) {
				return false;
}
}
		return true;
}
}

  bool  string_startswith_s(string_t *src, string_t *search)  {
if ( search->text_length > src->text_length) {
		return false;
}
for (long long int  i  = 0;  i  <  (int)search->text_length ;  i++) {
if ( src->text[i] != search->text[i]) {
			return false;
}
}
	return true;
}

  bool  string_startswith(string_t *src, const char *search)  {
	unsigned searchLength = strlen(search);
if ( searchLength > src->text_length) {
		return false;
}
for (long long int  i  = 0;  i  <  searchLength ;  i++) {
if ( src->text[i] != search[i]) {
			return false;
}
}
	return true;
}

  string_t*  string_substring_s(int startIndex, int endIndex, string_t *src)  {
	size_t totalAppend = endIndex - startIndex;
// Safety
if ( totalAppend > src->text_length) {
		throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
				"Unable to substring a string starting from index %d and ending %d",
				startIndex, endIndex);
}
// TODO: fix this
	string_t *newStr = custom_string_init(totalAppend + 2);
	strncpy(newStr->text, src->text + startIndex, totalAppend);
	newStr->text[totalAppend] = '\0';
	newStr->text_length += totalAppend + 1;

	return newStr;
}

  void  string_tolowercase_s(string_t *dest)  {
for (long long int  i  = 0;  i  <  (int)dest->text_length ;  i++) {
		dest->text[i] = tolower(dest->text[i]);
}
}

  bool  string_serialize(string_t *src, FILE *stream)  {
	fwrite(&src->text_length, sizeof(int), 1, stream);
	fwrite(src->text, sizeof(char), src->text_length, stream);
	return true;
}

  string_t*  string_deserialize(FILE *stream)  {
	int textLength;
	fread(&textLength, sizeof(int), 1, stream);

	string_t *str = custom_string_init(textLength + STRING_ALLOCATION_SIZE);
	fread(str->text, sizeof(char), textLength, stream);
	str->text_length = textLength;

	return str;
}

  void  string_reset(string_t *dest)  {
	dest->text[0] = '\0';
	dest->text_length = 0;
}

  void  string_free(void *dest)  {
	free(((string_t*) dest)->text);
// Free the structure itself
	free(((string_t*) dest));
}

// Memory related functions
static  void  string_meminspection(size_t addNum, string_t *subject)  {
	// +1 for accounting the null terminator
if ( subject->text_length + addNum + 1 >= subject->text_allocated_length) {
		size_t newSize = 1.5 * subject->text_allocated_length + addNum + 1;
		char *tempStr = (char *) realloc(subject->text, newSize);
		// Safety
if ( tempStr == NULL) {
			throw_exception(NULL_POINTER_EXCEPTION, -1,
					"Unable to allocate memory for string while doing meminspection!\n");
}
		subject->text = tempStr;
		subject->text_allocated_length = newSize;
}
}

// TODO: add a predefined message for errors like index out of bounds exception, etc.
  void  throw_exception(exception e, int lineNum, char *message, ...)  {
	va_list args;
	va_start(args, message);
	char cMessage[AVG_STRING_SIZE];
if ( lineNum == -1) {
		strncpy(cMessage, "Internal Error [", AVG_STRING_SIZE);
		vsnprintf(cMessage, AVG_STRING_SIZE, message, args);
		strcat(cMessage, "]"); 
} else {
		snprintf(cMessage, AVG_STRING_SIZE, "Line #%d", lineNum);
}
	va_end(args);

// Goes through the different types of error and prints out the appropriate message
	switch (e) {
	case ERRNO_EXCEPTION:
		perror(cMessage);
		break;
	case NULL_POINTER_EXCEPTION:
		fprintf(stderr, "%s: %s\n", cMessage, message);
		break;
	case INDEX_OUT_OF_BOUNDS_EXCEPTION:
		fprintf(stderr, "%s: %s\n", cMessage, message);
		break;
	default:
		break;
	}
}
