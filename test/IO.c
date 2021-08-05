
typedef struct {
Object *__extends;
long(*print) (char* text, ...) ;

} IO_t;

IO_t* IO_constructor(){
__extends = Object_constructor();
IO_t* __IO__obj = malloc(sizeof(IO_t));
__IO__obj->print=&IO_print;

return __IO__obj;
}
long IO_print(char* text, ...) {
IO->va_list args;
IO->va_start(args, text);
IO->vfprintf(stdout, text, args);
IO->va_end(args);
}
