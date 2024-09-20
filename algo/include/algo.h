#ifndef ALGO_H
#define ALGO_H

#include "logger.h"
#include "type.h"
#include "tree.h"

// ==========================================# PUBLIC #======================================================

void algo_free_state(void *state);

void algo_queue(Logger *logger, Exception *exception, User ***users, size_t *size);
void algo_pick(Logger *logger, Exception *exception, User **user, User **users, size_t size);
void algo_a_star_best(
  Logger *logger, Exception *exception, Tree *root, Tree **node, void *context,
  bool eval(void *stateA, void *stateB, void *context)
);
void algo_search(
  Logger *logger, Exception *exception,
  User *user, TimeTable *timetable, Assignment **schedule, size_t *size, size_t max_classroom_size, void *rules,
  bool in_rules(void *rules, TimeTable *timetable, Entry *block, ID class, ID user_id, bool is_student),
  size_t get_classrooms(ID **available_classrooms, TimeTable *timetable, Entry *block, ID class, size_t max_classroom_size)
);
void algo_update_tree(Logger *logger, Exception *exception, Tree **leaf, User *user, Assignment *schedule);
void algo_update_timetables(
  Logger *logger, Exception *exception,
  Tree *root, TimeTable ***possible_states, User **users, size_t *s_possible_states, size_t s_users
);

// ==========================================# PRIVATE #=====================================================

bool _algo_smaller_avail(User *userA, User *userB);
bool _algo_is_final_state(TimeTable *state, User **users, size_t size);

void _algo_check_null_users(Logger *logger, Exception *exception, User **users, size_t size);
void _algo_cut_null_users(Logger *logger, Exception *exception, User ***users, size_t *size);
void _algo_partition(Logger *logger, Exception *exception, User **users, int low, int high, int *pivot, bool sortby(User *userA, User *userB));
void _algo_recursion(Logger *logger, Exception *exception, User **users, int low, int high, bool sortby(User *userA, User *userB));
void _algo_quicksort(Logger *logger, Exception *exception, User **users, size_t size, bool sortby(User *userA, User *userB));

#endif // ALGO_H
