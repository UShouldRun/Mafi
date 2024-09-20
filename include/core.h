#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "algo.h"
#include "utils.h"

void read(
  Logger *logger, Report *report,
  char *read_file_name, size_t *depth, Rules *rules, TimeTable **root_state,
  ID **classrooms, ID **classes, User ***users,
  size_t *s_classrooms, size_t *s_classes, size_t *s_users
);

void store(Logger *logger, Report *report, char *write_file_name, TimeTable **timetables, size_t s_timetables);

void mafi(
  Logger *logger, Report *report,
  User ***users, TimeTable ***possible_timetables,
  size_t *s_users, size_t *s_possible_timetables,
  size_t depth, TimeTable *root_state, Rules *rules
);

#endif
