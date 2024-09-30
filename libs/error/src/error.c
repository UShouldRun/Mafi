#include "error.h"

static const size_t s_errors = 7;
static const char *errors[] = {
  "none",
  "IO Error",
  "Invalid Argument",
  "Null Pointer",
  "Out of Bounds",
  "Memory Error",
  "Unexpected Behaviour"
};

void _print_error(Error error, const char *msg, const char *file, const uint line) {
  fprintf(
    stderr,
    "\033[31m[ERROR]: [%s] %s at line %u in file %s\n\033[0m",
    errors[error < s_errors ? error : none], msg, line, file
  );
  fflush(stderr);
}

void _print_assertion_error(Error error, const char *msg, const char *file, const uint line) {
  fprintf(
    stderr,
    "\033[31m[ASSERTION FAILED: %s]: %s at line %u in file %s\n\033[0m",
    errors[error < s_errors ? error : none], msg, line, file
  );
  fflush(stderr);
}

void _print_exit(Error error, const uint line) {
  FILE *file = error ? stderr : stdout;
  fprintf(file, "Program exited with value %u in line %u\n", error, line);
  fflush(file);
}

void _throw_error(Error error, const char *msg, const char *file, const uint line) {
  _print_error(error, msg, file, line);
  _print_exit(error, line);
  exit(error);
}

void _throw_assertion(Error error, const char *msg, const char *file, const uint line) {
  _print_assertion_error(error, msg, file, line);
  _print_exit(error, line);
  exit(error);
}
