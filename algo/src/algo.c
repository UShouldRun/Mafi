#include "algo.h"

#define stop_if(condition) \
      if (condition) { \
        fprintf(stderr, "\033[33m[STOP]: %s at %s (%s: %u)\033[0m\n", #condition, __func__, __FILE__, __LINE__); \
        goto _return; \
      } \

#define check_if(condition, level, type) \
      if (condition) { \
        logger->updateException( \
          exception, level, type, logger_complete_message(#condition) \
        ); \
        goto _return; \
      } \

#define check_ptr(ptr_null_check) \
      if (ptr_null_check) { \
        logger->updateException( \
          exception, error, null_pointer, logger_complete_message(#ptr_null_check) \
        ); \
        goto _return; \
      } \

// ==========================================# PUBLIC #======================================================

void algo_free_state(void *state) {
  type_delete_timetable((TimeTable **)&state);
}

void algo_queue(Logger *logger, Exception *exception, User ***users, size_t *size) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(users == NULL);
  check_ptr(*users == NULL);
  check_ptr(size == NULL);

  _algo_check_null_users(logger, exception, *users, *size);
  stop_if(exception == NULL);
  check_if(exception->level > warning, exception->level, exception->type);
  if (exception->level == warning) {
    _algo_cut_null_users(logger, exception, users, size);
    stop_if(exception == NULL);
    check_if(exception->level != none, exception->level, exception->type);
  }

  _algo_quicksort(logger, exception, *users, *size, _algo_smaller_avail);

_return:
  return;
}

void algo_pick(Logger *logger, Exception *exception, User **user, User **users, size_t size) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(users == NULL);
  check_ptr(user == NULL);
  *user = *users;

_return:
  return;
}

void algo_a_star_best(
  Logger *logger, Exception *exception,
  Tree *root, Tree **node, void *context,
  bool eval(void *stateA, void *stateB, void *context)
) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(root == NULL);
  check_ptr(node == NULL);
  check_ptr(context == NULL);

  size_t size = 0;
  Tree **leaves = tree_library_get_leaves(root, &size);
  check_ptr(leaves == NULL);
  check_if(size == 0, error, unexp_behaviour);

  *node = *leaves;
  for (size_t i = 1; i < size; i++)
    if (eval((*node)->state, leaves[i]->state, context))
      *node = leaves[i];

  free(leaves);

_return:
  return;
}

void algo_search(
  Logger *logger, Exception *exception,
  User *user, TimeTable *timetable, Assignment **schedule, size_t *size, size_t max_classroom_size, void *rules,
  bool in_rules(void *rules, TimeTable *timetable, Entry *block, ID class, ID user_id, bool is_student),
  size_t get_classrooms(ID **available_classrooms, TimeTable *timetable, Entry *block, ID class, size_t max_classroom_size)
) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(user == NULL)
  check_ptr(type_get_avail(user) == NULL);
  check_ptr(type_get_classes(user) == NULL);
  check_ptr(timetable == NULL);
  check_ptr(schedule == NULL);
  check_ptr(size == NULL);
  check_ptr(rules == NULL);

  *size = 0;

  size_t capacity = 100;
  Assignment *temp = (Assignment *)malloc(capacity * sizeof(Assignment));
  check_if(temp == NULL, fatal, mem_error);

  const size_t len_avail = type_get_len_avail(user),
               len_classes = type_get_len_classes(user);
  
  const UserType user_type = type_get_is_student(user);
  const ID user_id = type_get_id(user);

  ID *user_classes = type_get_classes(user);
  check_ptr(user_classes == NULL);

  Entry *user_avail = type_get_avail(user);
  check_ptr(user_avail == NULL);

  for (size_t i = 0; i < len_avail; i++)
    for (size_t j = 0; j < len_classes; j++) {
      if (!in_rules(rules, timetable, &user_avail[i], user_classes[j], user_id, user_type))
        continue;

      ID *available_classrooms = NULL;
      size_t s_classes = get_classrooms(&available_classrooms, timetable, &user_avail[i], user_classes[j], max_classroom_size);
      check_if(available_classrooms == NULL && s_classes != 0, error, unexp_behaviour);
      check_if(available_classrooms != NULL && s_classes == 0, error, unexp_behaviour);
      if (available_classrooms == NULL)
        continue;
 
      if (*size + s_classes > capacity) {
        size_t new_capacity = 2 * (*size + s_classes);
        Assignment *new_temp = (Assignment *)realloc(temp, new_capacity * sizeof(Assignment));
        check_if(new_temp == NULL, fatal, mem_error);

        temp = new_temp;
        capacity = new_capacity;
      }

      for (size_t k = 0; k < s_classes; k++) {
        temp[*size].block = user_avail[i]; 
        temp[*size].class = user_classes[j];
        temp[(*size)++].classroom = available_classrooms[k];
      }

      free(available_classrooms);
      break;
    }

  if (*size > capacity) {
    Assignment *new_temp = (Assignment *)realloc(temp, (*size) * sizeof(Assignment));
    check_if(new_temp == NULL, fatal, mem_error);
    temp = new_temp;
  }

  *schedule = temp;
        
_return:
  if (exception->level != none && temp != NULL)
    free(temp);
  return;
}

void algo_update_tree(Logger *logger, Exception *exception, Tree **leaf, User *user, Assignment *schedule) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(leaf == NULL);
  check_ptr(user == NULL);
  check_ptr(schedule == NULL);

  TimeTable *leaf_state = (TimeTable *)(*leaf)->state,
            *new_state = type_copy_timetable(leaf_state);
 
  check_ptr(new_state == NULL);
  check_ptr(new_state->entries == NULL);

  TimeBlock timeblock = new_state->entries[schedule->block.i][schedule->block.j];
  ClassRoom *classroom = NULL;
  Class *class = NULL;

  for (size_t i = 0; i < timeblock.len; i++)
    if (timeblock.classrooms[i].id == schedule->classroom) {
      classroom = &timeblock.classrooms[i];
      break;
    }
  check_ptr(classroom == NULL);

  for (size_t i = 0; i < classroom->len; i++)
    if (classroom->classes[i].id == schedule->class) {
      class = &classroom->classes[i]; 
      break;
    }
  check_ptr(class == NULL);

  ID *temp = NULL;
  switch (user->type) {
    case student_type:
      temp = (ID *)realloc(class->students, (class->len_students + 1) * sizeof(ID));
      check_if(temp == NULL, fatal, mem_error);

      class->students = temp;
      class->students[class->len_students++] = type_get_id(user);
      break;

    case teacher_type:
      temp = (ID *)realloc(class->teachers, (class->len_teachers + 1) * sizeof(ID));
      check_if(temp == NULL, fatal, mem_error);

      class->teachers = temp;
      class->teachers[class->len_teachers++] = type_get_id(user);
      break;

    default:
      check_if(true, error, invalid_argument);
      goto _return;
  }

  *leaf = tree_library_add_child(*leaf, (void *)new_state);
  check_ptr(*leaf == NULL);

_return:
  return;
}

void algo_update_timetables(
  Logger *logger, Exception *exception,
  Tree *root, TimeTable ***possible_states, User **users,
  size_t *s_possible_states, size_t s_users
) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(root == NULL);
  check_ptr(possible_states == NULL);
  check_ptr(users == NULL); 
  check_ptr(s_possible_states == NULL);

  size_t s_leaves = 0;
  Tree **leaves = tree_library_get_leaves(root, &s_leaves);
  check_ptr(leaves == NULL);
  check_if(s_leaves == 0, error, unexp_behaviour);
 
  TimeTable **temp = (TimeTable **)malloc(s_leaves * sizeof(TimeTable *));
  check_if(temp == NULL, fatal, mem_error);

  for (size_t i = 0; i < s_leaves; i++)
    if (_algo_is_final_state((TimeTable *)leaves[i]->state, users, s_users)) {
      TimeTable *copy = type_copy_timetable((TimeTable *)leaves[i]->state);
      check_ptr(copy == NULL);
      temp[(*s_possible_states)++] = copy;
    }

  if (*s_possible_states == 0) {
    free(temp);
    temp = NULL;
  } else if (*s_possible_states != s_leaves) {
    TimeTable **new_temp = (TimeTable **)realloc(temp, (*s_possible_states) * sizeof(TimeTable *));
    check_if(new_temp == NULL, fatal, mem_error);
    temp = new_temp;
  }

  *possible_states = temp;

_return:
  if (exception->level != none && temp)
    free(temp);
  if (leaves)
    free(leaves);
  return;
}

// ==========================================# PRIVATE #=====================================================

bool _algo_smaller_avail(User *userA, User *userB) {
  return userA ? (
          userB ?
            (bool)(
              type_get_len_avail(userA) + type_get_len_assign(userA) < type_get_len_avail(userB) + type_get_len_assign(userB)
            )
          : false
        ) : false;
}

bool _algo_is_final_state(TimeTable *state, User **users, size_t size) {
  if (!state)
    return false;

  for (size_t i = 0; i < size; i++) {
    size_t s_classes = type_get_len_classes(users[i]);
    size_t s_assigns = type_get_assign_from_timetable(NULL, state, users[i]); 
    if (s_assigns != s_classes)
      return false;
  }
  return true;
}

void _algo_check_null_users(Logger *logger, Exception *exception, User **users, size_t size) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL)

  check_ptr(users == NULL);
  for (size_t i = 0; i < size; i++)
    check_ptr(users[i] == NULL);

_return:
  return;
}

void _algo_cut_null_users(Logger *logger, Exception *exception, User ***users, size_t *size) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(users == NULL);
  check_ptr(size == NULL);
  
  User **temp = (User **)malloc(*size*sizeof(User *));
  check_if(temp == NULL, fatal, mem_error);
  
  size_t _size = *size;
  for (size_t i = 0; i < _size; i++) {
    if ((*users)[i] == NULL) {
      (*size)--;
      check_if(*size == 0, error, index_out_of_bounds);
      continue;
    }
    temp[i] = (*users)[i];
  }

  temp = (User **)realloc(temp, (*size)*sizeof(User *));
  check_if(temp == NULL, fatal, mem_error);
  *users = temp;

_return:
  return;
}

void _algo_partition(Logger *logger, Exception *exception, User **users, int low, int high, int *pivot, bool sortby(User *userA, User *userB)) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(users == NULL);
  check_ptr(pivot == NULL);

  *pivot = high;
  int i = low - 1;
  User *temp;

  for (int j = low; j < high; j++)
    if (sortby(users[j], users[*pivot])) {
      temp = users[++i];
      users[i] = users[j];
      users[j] = temp;
    }

  temp = users[i+1];
  users[i+1] = users[*pivot];
  users[*pivot] = temp;

  *pivot = i + 1;

_return:
  return;
}

void _algo_recursion(Logger *logger, Exception *exception, User **users, int low, int high, bool sortby(User *userA, User *userB)) {
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  check_ptr(users == NULL);

  if (low >= high)
    goto _return;

  int pivot;
  _algo_partition(logger, exception, users, low, high, &pivot, sortby);
  check_ptr(exception == NULL);
  check_if(exception->level != none, error, unexp_behaviour);

  _algo_recursion(logger, exception, users, low, pivot - 1, sortby);
  check_ptr(exception == NULL);
  check_if(exception->level != none, error, unexp_behaviour);
  
  _algo_recursion(logger, exception, users, pivot + 1, high, sortby);

_return:
  return;
}

void _algo_quicksort(Logger *logger, Exception *exception, User **users, size_t size, bool sortby(User *userA, User *userB)) { 
  stop_if(logger == NULL);
  check_ptr(exception == NULL);

  _algo_recursion(logger, exception, users, 0, size-1, sortby);

_return:
  return;
}
