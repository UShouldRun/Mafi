#ifndef ERROR
#define ERROR

#include <stdio.h>
#include <stdlib.h>

typedef enum {
  none = 0,
  ioerror,
  inval_arg,
  nullptr,
  out_of_bounds,
  mem_error,
  unexp_behaviour
} Error;

// ==================================# PRIVATE #==================================

void _print_error(Error error, const char *msg, const char *file, const uint line);
void _print_assertion_error(Error error, const char *msg, const char *file, const uint line);
void _print_exit(Error error, const uint line);
void _throw_error(Error error, const char *msg, const char *file, const uint line);
void _throw_assertion(Error error, const char *msg, const char *file, const uint line);

// ==================================# PUBLIC #==================================

#define print_error(error, msg) _print_error(error, msg, __FILE__, __LINE__)
#define check(error, condition) !condition ? (void)0 : _print_error(error, #condition, __FILE__, __LINE__);
#define assert(error, condition) condition ? (void)0 : _throw_assertion(error, #condition, __FILE__, __LINE__);
#define panic(error, msg) _throw_error(error, msg, __FILE__, __LINE__)
#define print_exit_value(value) _print_exit(value, __LINE__)

#endif // !ERROR
