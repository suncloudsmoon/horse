#include "String__CLASS.h"
String_t* String_constructor() {
String_t* __String__obj = malloc(sizeof(String_t));
__String__obj->__extends = Object_constructor();
__String__obj->text=malloc(10 * sizeof(char));
__String__obj->length = 0;
__String__obj->allocatedLength = 10;
__String__obj->append=&String_append;
__String__obj->charAt=&String_charAt;
return __String__obj;
}

void String_append(String_t* __String__obj,char* extra) {

}
char String_charAt(String_t* __String__obj,long index) {
return  text;
}
