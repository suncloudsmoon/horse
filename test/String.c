
typedef struct {
Object *__extends;
char* text;
long length;
long allocatedLength;
void(*append) (char* extra) ;
char*(*charAt) (long index) ;

} String_t;

String_t* String_constructor() {
__extends = Object_constructor();
String_t* __String__obj = malloc(sizeof(String_t));
String->str = malloc(10 * sizeof(char));
String->length = 0;
String->allocatedLength = 10;
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
