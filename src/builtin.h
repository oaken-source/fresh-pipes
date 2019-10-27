#ifndef BUILTIN_H
#define BUILTIN_H

typedef int(*builtin_func_t)(int, char*[]);

builtin_func_t get_builtin (const char *command);

#endif
