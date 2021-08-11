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
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#define AVG_STRING_SIZE 5
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

// Exception Stuff
void throw_exception(exception e, int lineNum, char *message, ...);

// String stuff
static string_t* custom_string_init(size_t allocationSize);
static void string_meminspection(size_t addNum, string_t *subject);
string_t* string_init();
string_t* string_copyvalueof(char *text);
string_t* string_copyvalueof_s(string_t *src);
void string_append(string_t *dest, char *src);
void string_append_s(string_t *dest, string_t *src);
void string_appendchar(string_t *dest, char letter);
string_t** string_split(char delimiter, string_t *src);
bool string_equals(string_t *dest, char *src);
bool string_equals_s(string_t *dest, string_t *src);
bool string_equalsignorecase(string_t *dest, char *src);
bool string_equalsignorecase_s(string_t *dest, string_t *src);
bool string_startswith(string_t *src, char *search);
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

typedef struct {
	list_t *allLines; // List of all lines
	list_t *parsedLines; // List of parsed lines
	list_t *compiledLines;

	FILE *stream;
	FILE *outputFile;
} compiler_t;

// Compiler Functions
compiler_t* compiler_init(FILE *inputFile, FILE *outputFile);
void compiler_free(compiler_t *com);
void ignition(compiler_t *com);
void readAllLines(compiler_t *com);
static bool readLine(FILE *stream, string_t *line);
void parse(compiler_t *com);
static list_t* split(char delimiter, string_t *line);
static bool isSpecialCharacter(char alpha);
void compile(compiler_t *com);
static string_t* compileFunctionHeader(list_t *tokens);
int writeToFile(compiler_t *com);

const char *functionIdentifier = "function";

 int  main() {
	char *inputFilename = "src/compl.hr";
	char *outputFilename = "src/compl.c";
	FILE *input = fopen(inputFilename, "r");
	FILE *output = fopen(outputFilename, "w");

	compiler_t *com = compiler_init(input, output);
	ignition(com);
	compiler_free(com);
	return 0;
}

 compiler_t*  compiler_init(FILE *inputFile, FILE *outputFile) {
	compiler_t *com = malloc(sizeof(compiler_t));
	com->allLines = list_init();
	com->parsedLines = list_init();
	com->compiledLines = list_init();

	com->stream = inputFile;
	com->outputFile = outputFile;
	return com;
}

void compiler_free(compiler_t *com) {
	list_complete_free(&string_free, com->allLines);
	for (int i = 0; i < com->parsedLines->data_length; i++) {
		list_complete_free(&string_free, com->parsedLines->data[i]);
	}
	list_complete_free(&string_free, com->compiledLines);
	free(com);
}

void ignition(compiler_t *com) {
	printf("[ignition] program starting!\n");
	readAllLines(com);
	parse(com);
	compile(com);
	writeToFile(com);
}

void readAllLines(compiler_t *com) {
	string_t *line = string_init();
	while (!readLine(com->stream, line)) {
		list_add(string_copyvalueof_s(line), com->allLines);
		// Debug
		printf("[readAllLines] line from stream: %s\n", line->text);
		string_reset(line);
	}
	string_free(line);
}

static bool readLine(FILE *stream, string_t *line) {
	char letter;
	while ((letter = fgetc(stream)) != EOF && letter != '\n')
		string_appendchar(line, letter);
	return letter == EOF;
}

void parse(compiler_t *com) {
	for (int i = 0; i < com->allLines->data_length; i++) {
		string_t *existingLine = (string_t*) com->allLines->data[i];
		int firstLetter = strcspn(existingLine->text,
				"abcdefghijklmnopqrstuvwxyz0123456789#");
		string_t *initialClean = string_substring_s(firstLetter,
				existingLine->text_length, existingLine);
		list_add(split(' ', initialClean), com->parsedLines);

		// Debug
		printf("[parse] initialClean: %s\n", initialClean->text);
	}
}

static list_t* split(char delimiter, string_t *line) {
	list_t *output = list_init();
	string_t *temp = string_init();
	bool isSpecial = false;
	for (int i = 0; i < line->text_length; i++) {
		char alpha = line->text[i];
		if (isSpecialCharacter(alpha)) {
			isSpecial = !isSpecial;
		} else if (!isSpecial && alpha == delimiter) {
			list_add(string_copyvalueof_s(temp), output);
			string_reset(temp);
		}
		string_appendchar(temp, alpha);
	}
	list_add(temp, output);
	return output;
}

static bool isSpecialCharacter(char alpha) {
	return alpha == '"' || alpha == '\'' || alpha == '(' || alpha == ')';
}

void compile(compiler_t *com) {
	string_t *parsed;
	for (int i = 0; i < com->parsedLines->data_length; i++) {
		list_t *tokens = com->parsedLines->data[i];
		if (string_startswith((string_t*) tokens->data[0],
				functionIdentifier)) {
			parsed = compileFunctionHeader(tokens);
		} else {
			parsed = (string_t*) com->allLines->data[i];
		}
		list_add(string_copyvalueof_s(parsed), com->compiledLines);
		// Debug
		printf("[compile] parsed: %s\n",
				((string_t*) com->compiledLines->data[i])->text);
	}
}

static string_t* compileFunctionHeader(list_t *tokens) {
	// unction something() returns int
	char functHeader[1000];
	snprintf(functHeader, 1000, "%s %s {", ((string_t*) tokens->data[3])->text,
			((string_t*) tokens->data[1])->text);
	return string_copyvalueof(functHeader);
}

int writeToFile(compiler_t *com) {
	FILE *output = com->outputFile;
	for (int i = 0; i < com->compiledLines->data_length; i++) {
		fprintf(output, "%s\n",
				((string_t*) com->compiledLines->data[i])->text);		
	}
	printf("Successfully wrote to File!");
	return fclose(output);
}

////// LIBRARIES //////

list_t* list_init() {
	list_t *list = malloc(sizeof(list_t));
	list->data = (void**) malloc(LIST_MANAGER_ALLOC_SIZE * sizeof(void*));

	list->data_length = 0;
	list->data_allocated_length = LIST_MANAGER_ALLOC_SIZE;
}

static list_t* custom_list_init(size_t mallocSize) {
	list_t *list = malloc(sizeof(list_t));
	list->data = (void**) malloc(mallocSize * sizeof(void*));

	list->data_length = 0;
	list->data_allocated_length = mallocSize;
}

void list_add(void *item, list_t *list) {
	list_meminspector(1, list);
	list->data[list->data_length] = item;
	list->data_length++;
}

void list_remove(int index, list_t *list) {
	// TODO: find a more efficient implementation of this
	for (int i = index; i < list->data_length - 1; i++)
		list->data[i] = list->data[i + 1];

	list->data_length--;
}

void list_complete_remove(void (*indivfree)(void*), int index, list_t *list) {
	(*indivfree)(list->data[index]); // frees it from the respective free method for the unknown type data
	list_remove(index, list);
}

void list_clear(list_t *list) {
	list->data_length = 0;
}

bool list_equals(void *destComp, int index,
bool (*equalsComparator)(void*, void*), list_t *list) {
	if (index < 0 || index >= list->data_length)
		throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
				"Tried to access a list in index %d that was out of bounds!",
				index);

	return (*equalsComparator)(destComp, list->data[index]);
}

bool list_contains(void *destComp, bool (*equalsComparator)(void*, void*),
		list_t *list) {
	for (int i = 0; i < list->data_length; i++)
		if ((*equalsComparator)(destComp, list->data[i]))
			return true;
	return false;
}

void list_serialize(void (*indiv)(void*, FILE*), FILE *stream, list_t *list) {
	fwrite(list->data_length, sizeof(int), 1, stream);
	for (int i = 0; i < list->data_length; i++)
		(*indiv)(list->data[i], stream);
}

list_t* list_deserialize(void* (*indivreverse)(FILE*), FILE *stream) {
	int arrayLength;
	fread(&arrayLength, sizeof(int), 1, stream);

	list_t *list = custom_list_init(arrayLength);
	for (int i = 0; i < arrayLength; i++)
		list_add((*indivreverse)(stream), list);

	return list;
}

void list_free(list_t *list) {
	free(list->data);
	free(list);
}

void list_complete_free(void (*indivfree)(void*), list_t *list) {
	for (int i = 0; i < list->data_length; i++)
		(*indivfree)(list->data[i]);
	list_free(list);
}

static void list_meminspector(size_t addNum, list_t *subject) {
	if (subject->data_length + addNum >= subject->data_allocated_length) {
		size_t newSize = 1.5 * subject->data_allocated_length + addNum;
		void **new_ptr = (void**) realloc(subject->data,
				newSize * sizeof(void*));
		if (new_ptr == NULL)
			throw_exception(NULL_POINTER_EXCEPTION, -1,
					"Unable to allocate memory for list with length %d!",
					subject->data_length);

		subject->data = new_ptr;
		subject->data_allocated_length = newSize;
	}
}

string_t* string_init() {
	string_t *str = malloc(sizeof(string_t));

	str->text = malloc(STRING_ALLOCATION_SIZE * sizeof(char));
	str->text[0] = '\0';

	str->text_length = 0;
	str->text_allocated_length = STRING_ALLOCATION_SIZE;

	return str;
}

static string_t* custom_string_init(size_t allocationSize) {
	string_t *str = malloc(sizeof(string_t));

	str->text = malloc(allocationSize);
	str->text[0] = '\0';

	str->text_length = 0;
	str->text_allocated_length = allocationSize;

	return str;
}

string_t* string_copyvalueof(char *src) {
	int srcLength = strlen(src);

	string_t *newStr = custom_string_init(srcLength + STRING_ALLOCATION_SIZE);
	strncpy(newStr->text, src, srcLength);
	newStr->text_length = srcLength;

	return newStr;
}

string_t* string_copyvalueof_s(string_t *src) {
	string_t *dest = malloc(sizeof(string_t));
	dest->text = strdup(src->text);
	dest->text_length = src->text_length;
	dest->text_allocated_length = src->text_length + 1;

	return dest;
}

void string_append(string_t *dest, char *src) {
	int srcLength = strlen(src);
	string_meminspection(srcLength, dest);

	strncat(dest->text, src, srcLength);
	dest->text_length += srcLength;
}

void string_append_s(string_t *dest, string_t *src) {
	string_meminspection(src->text_length, dest);
	strncat(dest->text, src->text, src->text_length);
	dest->text_length += src->text_length;
}

void string_appendchar(string_t *dest, char letter) {
	string_meminspection(1, dest);
	char text[2];
	text[0] = letter;
	text[1] = '\0';
	strcat(dest->text, text);
	dest->text_length++;
}

string_t** string_split(char delimiter, string_t *src) {
	// Safety
	// If the string is length 2 or less, then it is not possible to split the string
	if (src->text_length <= 2)
		throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
				"Unable to substring a string with %d length!",
				src->text_length);

	string_t **strList = malloc(2 * sizeof(string_t));
	strList[0] = custom_string_init(src->text_length / 2);
	strList[1] = custom_string_init(src->text_length / 2);

	char delimiterText[2];
	delimiterText[0] = delimiter;
	delimiterText[1] = '\0';
	int splitIndex = strcspn(src->text, delimiterText);

	if (splitIndex == src->text) {
		return NULL;
	}

	for (int i = 0; i < splitIndex; i++) {
		string_appendchar(strList[0], src->text[i]);
	}
	for (int i = splitIndex + 1; i < src->text_length; i++) {
		string_appendchar(strList[1], src->text[i]);
	}

	return strList;
}

bool string_equals(string_t *dest, char *src) {
	int srcLength = strlen(src);
	if (dest->text_length != srcLength)
		return false;
	else
		return strncmp(dest->text, src, srcLength) == 0;
}

bool string_equals_s(string_t *dest, string_t *src) {
	if (dest->text_length != src->text_length)
		return false;
	else
		return strncmp(dest->text, src, src->text_length) == 0;
}

bool string_equalsignorecase(string_t *dest, char *src) {
	if (dest->text_length != strlen(src)) {
		return false;
	} else {
		for (int i = 0; i < dest->text_length; i++)
			if (tolower(dest->text[i]) != tolower(src[i]))
				return false;
		return true;
	}
}

bool string_equalsignorecase_s(string_t *dest, string_t *src) {
	if (dest->text_length != src->text_length) {
		return false;
	} else {
		for (int i = 0; i < dest->text_length; i++)
			if (tolower(dest->text[i]) != tolower(src->text[i]))
				return false;
		return true;
	}
}

bool string_startswith_s(string_t *src, string_t *search) {
	return strcspn(src->text, search->text) != src->text_length;
}

bool string_startswith(string_t *src, char *search) {
	int searchLength = strlen(search);
	if (src->text_length != searchLength) {
		return false;
	}
	for (int i = 0; i < src->text_length; i++)
		if (src->text[i] != search[i])
			return false;
	return true;
}

//char string_indexof_s(int index, string_t *src) {
//
//}

string_t* string_substring_s(int startIndex, int endIndex, string_t *src) {
	size_t totalAppend = endIndex - startIndex;
// Safety
	if (totalAppend < 0 || totalAppend > src->text_length) {
		throw_exception(INDEX_OUT_OF_BOUNDS_EXCEPTION, -1,
				"Unable to substring a string starting from index %d and ending %d",
				startIndex, endIndex);
	}
// TODO: fix this
	string_t *newStr = custom_string_init(totalAppend + 1);
	strncpy(newStr->text, src->text + startIndex, totalAppend);
	newStr->text[totalAppend] = '\0';
	newStr->text_length += totalAppend;

	return newStr;
}

void string_tolowercase_s(string_t *dest) {
	for (int i = 0; i < dest->text_length; i++)
		dest->text[i] = tolower(dest->text[i]);
//	for (char *letter = dest->text; *letter; letter++)
//		*letter = tolower(*letter);
}

bool string_serialize(string_t *src, FILE *stream) {
	fwrite(&src->text_length, sizeof(int), 1, stream);
	fwrite(src->text, sizeof(char), src->text_length, stream);
}

string_t* string_deserialize(FILE *stream) {
	int textLength;
	fread(&textLength, sizeof(int), 1, stream);

	string_t *str = custom_string_init(textLength + STRING_ALLOCATION_SIZE);
	fread(str->text, sizeof(char), textLength, stream);
	str->text_length = textLength;

	return str;
}

void string_reset(string_t *dest) {
	dest->text[0] = '\0';
	dest->text_length = 0;
}

void string_free(void *dest) {
// Free string inside dest
	free(((string_t*) dest)->text);
// Free the structure itself
	free(((string_t*) dest));
}

// Memory related functions
static void string_meminspection(size_t addNum, string_t *subject) {
	// +1 for accounting the null terminator
	if (subject->text_length + addNum + 1 >= subject->text_allocated_length) {
		size_t newSize = 1.5 * subject->text_allocated_length + addNum + 1;
		char *tempStr = (char *) realloc(subject->text, newSize);
		// Safety
		if (tempStr == NULL)
			throw_exception(NULL_POINTER_EXCEPTION, -1,
					"Unable to allocate memory for string while doing meminspection!\n");
		subject->text = tempStr;
		subject->text_allocated_length = newSize;
	}
}

void throw_exception(exception e, int lineNum, char *message, ...) {
// TODO: add a predefined message for errors like index out of bounds exception, etc.

	va_list args;
	va_start(args, message);
	char cMessage[AVG_STRING_SIZE];
	if (lineNum == -1) {
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
