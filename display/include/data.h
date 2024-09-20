#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "error.h"

#ifndef CHECK_ERROR
#define CHECK_ERROR

#define check_error(error, condition) \
      do { \
        if (condition) { \
          _print_error(error, #condition, __FILE__, __LINE__); \
          return error; \
        } \
      } while (0);

#endif

Error read_mw(FILE *file, TimeTable **timetables, size_t *s_timetables);

size_t append(uint data, uint **array, size_t len);
bool included(ID user, ID *users, size_t s_users);
int in_schedule(Assignment *assign, size_t s_assign, size_t i, size_t j);
size_t get_users(TimeTable *timetable, ID **users);

#endif
