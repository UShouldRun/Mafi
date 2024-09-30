#include <stdio.h>
#include <stdlib.h>

#define none logger_none
#define unexp_behaviour logger_unexp_behaviour
#define mem_error logger_mem_error

#include "algo.h"

#undef none
#undef unexp_behaviour
#undef mem_error

#include "testing.h"
#include "error.h"

int str_to_int(char *str) {
  if (!str)
    return 0;

  int x = 0;
  for (; *str != '\0'; x = 10*x + *str - '0', str++)
    if (*str < '0' || *str > '9')
      return 0;

  return x;
}

size_t str_len(char *str) {
  if (!str)
    return 0;

  size_t len = 0;
  for (; *str != '\0'; len++, str++);
  return len;
}

bool in_array(uint x, uint *array, size_t s_array) {
  if (!array)
    return false;

  for (size_t i = 0; i < s_array; i++)
    if (array[i] == x)
      return true;

  return false;
}

int arrayset(uint *array, uint value, size_t s_array) {
  if (!array)
    return 1;
  if (!s_array)
    return 2;
  for (size_t i = 0; i < s_array; i++)
    array[i] = value;
  return 0;
}

void update_progress_bar(size_t iteration, const size_t s_tests) {
  const size_t width = 50,
               pos = (iteration + 1) / width * s_tests;

  printf("\r[");
  for (size_t i = 0; i < width; i++)
    printf("%s", i < pos ? "=" : " ");

  printf("] %3zu%% %zu/%zu", (iteration + 1) * 100 / s_tests, iteration + 1, s_tests);
  fflush(stdout);
  
  if (iteration + 1 == s_tests)
    printf("\n");
}

#define check_fail(error, condition) \
          do { \
            if (condition) { \
              _print_error(error, #condition, __FILE__, __LINE__); \
              status = TEST_FAIL; \
              goto _return; \
            } \
          } while (0) \

#define check_error(error, condition) \
          do { \
            if (condition) { \
              _print_error(error, #condition, __FILE__, __LINE__); \
              status = TEST_ERROR; \
              goto _return; \
            } \
          } while (0) \


TestStatus test_algo_queue_and_algo_pick() {
  TestStatus status = TEST_PASS;

  size_t min_classes = 5, s_classes = rand() % 6 + min_classes;
  ID *classes = (ID *)malloc(s_classes * sizeof(ID));
  check_error(mem_error, classes == NULL);
  for (size_t i = 0; i < s_classes; i++)
    classes[i] = i + 1;

  size_t s_users = rand() % 20 + 5;
  User **users = (User **)malloc(s_users * sizeof(User *)); 
  check_error(mem_error, users == NULL);

  for (size_t i = 0; i < s_users; i++) {
    UserType type = (UserType)(rand() % 2 + 1);
    ID id = i,
       *user_classes = (ID *)malloc(min_classes * sizeof(ID));
    check_error(mem_error, user_classes == NULL);
    check_error(unexp_behaviour, arrayset(user_classes, 0, min_classes));

    for (size_t j = 0; j < min_classes; j++) {
      ID x;
      do {
        x = classes[rand() % s_classes];
      } while (in_array(x, user_classes, min_classes));
      user_classes[j] = x;
    }

    size_t s_avail = rand() % 10 + 6;
    Entry *avail = (Entry *)malloc(s_avail * sizeof(Entry));
    for (size_t j = 0; j < s_avail; j++) {
      bool is_chosen_block = false;
      size_t entry_i, entry_j;
      do {
        is_chosen_block = false;

        entry_i = rand() % 4;
        entry_j = rand() % 5;

        for (size_t k = 0; k < j; k++)
          if (avail[k].i == entry_i && avail[k].j == entry_j) {
            is_chosen_block++;
            break;
          }
      } while (is_chosen_block);
      avail[j].i = entry_i;
      avail[j].j = entry_j;
    }
      
    users[i] = type_create_user(type, id, user_classes, avail, min_classes, s_avail);
    check_error(nullptr, users[i] == NULL);
  }

  Logger logger;
  initLogger(&logger, stdout);

  Exception *exception = logger.createException(logger_none, nil);
  algo_queue(&logger, exception, &users, &s_users);
  check_error(nullptr, exception == NULL);

  if (exception->level != logger_none) {
    logger.writeException(exception, stderr);
    check_error(unexp_behaviour, exception->level > warning);
  }

  size_t min_avail = type_get_size_avail(users[0]);
  for (size_t i = 1; i < s_users; i++) {
    size_t s_avail = type_get_size_avail(users[i]);
    check_fail(unexp_behaviour, min_avail > s_avail);
    min_avail = s_avail;
  }

  User *picked_user = NULL;
  algo_pick(&logger, exception, &picked_user, users, s_users);
  check_error(nullptr, exception == NULL);

  if (exception->level != logger_none) {
    logger.writeException(exception, stderr);
    check_error(unexp_behaviour, exception->level > warning);
  }

  check_fail(unexp_behaviour, picked_user != users[0]);

_return:
  if (exception)
    free(exception);
  if (classes)
    free(classes);
  if (users) {
    for (size_t i = 0; i < s_users; i++) {
      type_delete_user(&users[i]);
      check(unexp_behaviour, users[i]);
    }
    free(users);
  }
    
  return status;
}

bool greater_than(void *a, void *b, void *context) {
  if (a == NULL)
    return false;
  if (b == NULL)
    return false;
  return *(int *)a > *(int *)b;
}

TestStatus test_algo_a_star_best() {
  TestStatus status = TEST_PASS;
  
  uint root_state = 0;
  Tree *root = tree_library_create_root((void *)&root_state); 
  check_error(nullptr, root == NULL);

  size_t s_children = rand() % 3 + 1;
  uint *root_children_states = (uint *)malloc(s_children * sizeof(uint));
  check_error(mem_error, root_children_states == NULL);
  for (size_t i = 0; i < s_children; i++) {
    root_children_states[i] = i + 1;
    root = tree_library_add_child(root, (void *)&root_children_states[i]);
    check_error(nullptr, root == NULL);
  }

  size_t chosen_child = rand() % s_children,
         s_grandchildren = rand() % 3 + 1;
  uint *root_grandchildren_states = (uint *)malloc(s_grandchildren * sizeof(uint));
  check_error(mem_error, root_grandchildren_states == NULL);
  for (size_t i = 0; i < s_grandchildren; i++) {
    root_grandchildren_states[i] = s_children + i + 1;
    root->children[chosen_child] = tree_library_add_child(root->children[chosen_child], (void *)&root_grandchildren_states[i]);
    check_error(nullptr, root->children[chosen_child] == NULL);
  }

  Tree *best_node = NULL;
  int context = 0;

  Logger logger;
  initLogger(&logger, stdout);

  Exception *exception = logger.createException(logger_none, nil);
  algo_a_star_best(&logger, exception, root, &best_node, &context, greater_than);
  check_error(nullptr, exception == NULL);
  if (exception->level != logger_none) {
    logger.writeException(exception, stderr);
    check_error(unexp_behaviour, exception->level > warning);
  }

_return:
  tree_library_delete_tree(&root, NULL);
  check_error(unexp_behaviour, root);

  free(root_children_states);
  free(root_grandchildren_states);

  logger.deleteException(exception);

  return status;
}

bool in_rules(void *rules, TimeTable *timetable, Entry *block, ID class, ID user_id, bool is_student) {
  return true;
}

size_t get_classrooms(ID **available_classrooms, TimeTable *timetable, Entry *block, ID class, size_t max_size) {
  if (!timetable)
    return 0;
  if (!block)
    return 0;

  bool valid_entry = (bool)((block->i >= 0 && block->i < timetable->n) && (block->j >= 0 && block->j < timetable->m));
  if (!valid_entry)
    return 0;
  
  const size_t s_classrooms = timetable->entries[block->i][block->j].s_classrooms;
  ClassRoom *classrooms = timetable->entries[block->i][block->j].classrooms;
  if (!classrooms)
    return 0;

  size_t capacity = 10;
  ID *temp = (ID *)malloc(capacity * sizeof(ID));
  if (!temp) {
    *available_classrooms = NULL;
    return 0;
  }

  size_t s_avail_classrooms = 0;
  for (size_t i = 0; i < s_classrooms; i++) {
    if (type_get_classroom_current_capacity(&classrooms[i]) > max_size)
      continue;

    const size_t s_classes = classrooms[i].s_classes;
    Class *classes = classrooms[i].classes;
    if (!classes)
      continue;

    uint has_class = 0;
    for (size_t j = 0; j < s_classes; j++)
      if (classes[j].id == class) {
        has_class++;
        break;
      }

    if (!has_class)
      continue;

    if (s_avail_classrooms == capacity) {
      ID *new_temp = (ID *)realloc(temp, (2 * capacity)*sizeof(ID));
      if (!new_temp) {
        *available_classrooms = NULL;
        return 0;
      }
      temp = new_temp;
      capacity *= 2;
    }
    temp[s_avail_classrooms++] = classrooms[i].id;
  }

  if (s_avail_classrooms != capacity) {
    ID *new_temp = (ID *)realloc(temp, s_avail_classrooms*sizeof(ID));
    if (!new_temp) {
      *available_classrooms = NULL;
      return 0;
    }
    temp = new_temp;
  }

  *available_classrooms = temp;
  return s_avail_classrooms;
}

TestStatus test_algo_search() {
  TestStatus status = TEST_PASS;

  const size_t min_classes = 2, s_classes = rand() % 3 + min_classes;
  ID *classes_id = (ID *)malloc(s_classes * sizeof(ID));
  check_error(mem_error, classes_id == NULL);
  for (size_t i = 0; i < s_classes; i++)
    classes_id[i] = i + 1;

  const size_t s_classrooms = rand() % 3 + 2;
  const size_t n = 4, m = 5;
  TimeBlock **entries = (TimeBlock **)malloc(n * sizeof(TimeBlock *));
  check_error(mem_error, entries == NULL);
  for (size_t i = 0; i < n; i++) {
    entries[i] = (TimeBlock *)malloc(m * sizeof(TimeBlock));
    check_error(mem_error, entries[i] == NULL);
    
    for (size_t j = 0; j < m; j++) {
      Entry entry = { .i = i, .j = j };

      ClassRoom *classrooms = (ClassRoom *)malloc(s_classrooms * sizeof(ClassRoom));
      check_error(mem_error, classrooms == NULL);
      for (size_t k = 0; k < s_classrooms; k++) {
        classrooms[k].id = k + 1;
        classrooms[k].s_classes = min_classes;

        Class *classes = (Class *)malloc(min_classes * sizeof(Class));
        check_error(mem_error, classes == NULL);

        for (size_t l = 0; l < min_classes; l++) {
          bool is_chosen_class = false;
          ID x;
          do {
            is_chosen_class = false;
            x = classes_id[rand() % s_classes];

            for (size_t m = 0; m < l; m++)
              if (classes[m].id == x) {
                is_chosen_class++;
                break;
              }
          } while (is_chosen_class);

          classes[l].id = x;
          classes[l].s_students = 0;
          classes[l].s_teachers = 0;
          classes[l].students = NULL;
          classes[l].teachers = NULL;
        }

        classrooms[k].classes = classes;
      }

      TimeBlock *block = type_create_timeblock(&entry, classrooms, s_classrooms);
      check_error(nullptr, block == NULL);

      entries[i][j] = *block;
      free(block);
    }
  }

  TimeTable *timetable = type_create_timetable(entries, n, m);
  check_error(nullptr, timetable == NULL);

  UserType type = student_type;
  ID id = 1;

  ID *user_classes = (ID *)malloc(min_classes * sizeof(ID));
  check_error(mem_error, user_classes == NULL);
  check_error(unexp_behaviour, arrayset(user_classes, 0, min_classes));

  for (size_t i = 0; i < min_classes; i++) {
    ID x;
    do {
      x = classes_id[rand() % s_classes];
    } while (in_array(x, user_classes, min_classes));
    user_classes[i] = x;
  }
  
  size_t s_avail = rand() % 5 + 4;
  Entry *avail = (Entry *)malloc(s_avail * sizeof(Entry));
  check_error(mem_error, avail == NULL);

  for (size_t i = 0; i < s_avail; i++) {
    bool is_chosen_block = false;
    Entry block;

    do {
      is_chosen_block = false;

      block.i = rand() % n;
      block.j = rand() % m;

      for (size_t j = 0; j < i; j++)
        if (avail[j].i == block.i && avail[j].j == block.j) {
          is_chosen_block++;
          break;
        }
    } while (is_chosen_block);

    avail[i].i = block.i;
    avail[i].j = block.j;
  }
  
  User *user = type_create_user(type, id, user_classes, avail, min_classes, s_avail);

  const size_t max_classroom_size = rand() % 3 + 2;
  int rules = 0;

  Assignment *assign = NULL;
  size_t s_assign = 0;

  Logger logger;
  initLogger(&logger, stdout);

  Exception *exception = logger.createException(logger_none, nil);
  algo_search(&logger, exception, user, timetable, &assign, &s_assign, max_classroom_size, &rules, in_rules, get_classrooms);
  check_error(nullptr, exception == NULL);
  if (exception->level != logger_none) {
    logger.writeException(exception, stderr);
    check_error(unexp_behaviour, exception->level != logger_none);
  }

_return:
  free(classes_id);

  type_delete_timetable(&timetable);
  check_error(unexp_behaviour, timetable);

  type_delete_user(&user);
  check_error(unexp_behaviour, user);

  free(assign);

  logger.deleteException(exception);
  return status;
}

TestStatus test_algo_update_tree() {
  TestStatus status = TEST_PASS;

  const size_t min_classes = 2;
  ID *classes_id = (ID *)malloc(min_classes * sizeof(ID));
  check_error(mem_error, classes_id == NULL);
  for (size_t i = 0; i < min_classes; i++)
    classes_id[i] = i + 1;

  const size_t n = 2, m = 2, s_classrooms = rand() % 3 + 2;
  TimeBlock **entries = (TimeBlock **)malloc(n * sizeof(TimeBlock *));
  check_error(mem_error, entries == NULL);

  for (size_t i = 0; i < n; i++) {
    entries[i] = (TimeBlock *)malloc(m * sizeof(TimeBlock));
    check_error(mem_error, entries[i] == NULL);
    
    for (size_t j = 0; j < m; j++) {
      Entry entry = { .i = i, .j = j };

      ClassRoom *classrooms = (ClassRoom *)malloc(s_classrooms * sizeof(ClassRoom));
      check_error(mem_error, classrooms == NULL);
      for (size_t k = 0; k < s_classrooms; k++) {
        classrooms[k].id = k + 1;
        classrooms[k].s_classes = min_classes;

        Class *classes = (Class *)malloc(min_classes * sizeof(Class));
        check_error(mem_error, classes == NULL);

        for (size_t l = 0; l < min_classes; l++) {
          classes[l].id = classes_id[l];
          classes[l].s_students = 0;
          classes[l].s_teachers = 0;
          classes[l].students = NULL;
          classes[l].teachers = NULL;
        }

        classrooms[k].classes = classes;
      }

      TimeBlock *block = type_create_timeblock(&entry, classrooms, s_classrooms);
      check_error(nullptr, block == NULL);

      entries[i][j] = *block;
      free(block);
    }
  }

  TimeTable *timetable = type_create_timetable(entries, n, m);
  check_error(nullptr, timetable == NULL);

  UserType type = rand() % 2 + 1;
  ID id = 1;

  ID *user_classes = (ID *)malloc(min_classes * sizeof(ID));
  check_error(mem_error, user_classes == NULL);
  check_error(unexp_behaviour, arrayset(user_classes, 0, min_classes));

  for (size_t i = 0; i < min_classes; i++)
    user_classes[i] = classes_id[i];
  
  size_t s_avail = rand() % 3 + 2;
  Entry *avail = (Entry *)malloc(s_avail * sizeof(Entry));
  check_error(mem_error, avail == NULL);

  for (size_t i = 0; i < s_avail; i++) {
    bool is_chosen_block = false;
    Entry block;

    do {
      is_chosen_block = false;

      block.i = rand() % n;
      block.j = rand() % m;

      for (size_t j = 0; j < i; j++)
        if (avail[j].i == block.i && avail[j].j == block.j) {
          is_chosen_block++;
          break;
        }
    } while (is_chosen_block);

    avail[i].i = block.i;
    avail[i].j = block.j;
  }
  
  User *user = type_create_user(type, id, user_classes, avail, min_classes, s_avail);
  check_error(nullptr, user == NULL);

  Tree *root = tree_library_create_root((void *)timetable);
  check_error(nullptr, root == NULL);

  Entry *user_avail = type_get_avail(user);
  Assignment assign = {
    .block = user_avail[rand() % type_get_size_avail(user)],
    .classroom = rand() % s_classrooms + 1,
    .class = rand() % min_classes + 1
  };

  Logger logger;
  initLogger(&logger, stdout);

  Exception *exception = logger.createException(logger_none, nil);
  algo_update_tree(&logger, exception, &root, user, &assign);
  check_fail(nullptr, exception == NULL);
  if (exception->level != logger_none) {
    logger.writeException(exception, stderr);
    check_error(unexp_behaviour, exception->level != logger_none);
  }

  check_error(nullptr, root == NULL);
  check_fail(nullptr, root->children == NULL);
  check_fail(unexp_behaviour, root->s_children != 1);
  check_fail(nullptr, root->children[0]->state == NULL);

  TimeTable *new_timetable = (TimeTable *)root->children[0]->state;

  check_fail(unexp_behaviour, new_timetable->n != timetable->n);
  check_fail(unexp_behaviour, new_timetable->m != timetable->m);
  check_fail(nullptr, new_timetable->entries == NULL);
  check_fail(unexp_behaviour, new_timetable->entries[assign.block.i][assign.block.j].s_classrooms != s_classrooms);
  check_fail(nullptr, new_timetable->entries[assign.block.i][assign.block.j].classrooms == NULL);

  ClassRoom *classrooms = new_timetable->entries[assign.block.i][assign.block.j].classrooms;
  ClassRoom assign_classroom;
  bool in_classroom = false;

  for (size_t i = 0; i < s_classrooms; i++)
    if (classrooms[i].id == assign.classroom) {
      assign_classroom = classrooms[i];
      in_classroom++;
      break;
    }
  check_fail(unexp_behaviour, in_classroom == false);
  check_fail(unexp_behaviour, assign_classroom.s_classes != min_classes);
  check_fail(nullptr, assign_classroom.classes == NULL);

  Class assign_class;
  bool in_class = false;
  for (size_t i = 0; i < assign_classroom.s_classes; i++)
    if (assign_classroom.classes[i].id == assign.class) {
      assign_class = assign_classroom.classes[i];
      in_class++;
      break;
    }
  check_fail(unexp_behaviour, in_class == false);
  check_fail(unexp_behaviour, user->type == student_type && assign_class.s_students == 0);
  check_fail(nullptr, user->type == student_type && assign_class.students == NULL);
  check_fail(unexp_behaviour, user->type == teacher_type && assign_class.s_teachers == 0);
  check_fail(nullptr, user->type == teacher_type && assign_class.teachers == NULL);

  bool is_assigned_correctly = false;
  ID user_id = type_get_id(user);
  if (user->type == student_type) {
    for (size_t i = 0; i < assign_class.s_students; i++)
      if (assign_class.students[i] == user_id) {
        is_assigned_correctly++;
        break;
      }
  } else {
    for (size_t i = 0; i < assign_class.s_teachers; i++)
      if (assign_class.teachers[i] == user_id) {
        is_assigned_correctly++;
        break;
      }
  } 
  check_fail(unexp_behaviour, is_assigned_correctly == false);

_return:
  free(classes_id);

  type_delete_user(&user);
  check_error(unexp_behaviour, user);

  tree_library_delete_tree(&root, algo_free_state);

  logger.deleteException(exception);
  return status;
}

void add_result(TestCase *test_case, TestResult *test_result, const size_t s_tests) {
  clock_t test_duration = 0;

  for (size_t i = 0; i < s_tests; i++) {
    update_progress_bar(i, s_tests);

    clock_t start, end;
    start = clock();
    test_result->status = test_case->test_func();
    end = clock();

    test_duration += end - start;
    if (test_result->status != TEST_PASS)
      break;
  }

  test_result->message = test_case->name;
  test_result->duration = ((double)test_duration / CLOCKS_PER_SEC * 1000);
}

void print_result(TestResult *result, const size_t s_tests) {
  switch (result->status) {
    case TEST_PASS:
      fprintf(
        stdout,
        "\033[32m[PASS]: %s, test duration - %.3f ms, average duration - %.6f ms\n\033[0m",
        result->message, result->duration, result->duration / s_tests
      );
      break;
    case TEST_SKIP:
      fprintf(
        stdout,
        "\033[33m[SKIP]: %s, test duration - %.3f ms, average duration - %.6f ms\n\033[0m",
        result->message, result->duration, result->duration / s_tests
      );
      break;
    case TEST_FAIL:
      fprintf(
        stderr,
        "\033[33m[FAIL]: %s, test duration - %.3f ms, average duration - %.6f ms\n\033[0m",
        result->message, result->duration, result->duration / s_tests
      );
      break;
    case TEST_ERROR:
      fprintf(
        stderr,
        "\033[31m[ERROR]: %s, test duration - %.3f ms, average duration - %.6f ms\n\033[0m",
        result->message, result->duration, result->duration / s_tests);
      break;
    default:
      return;
  }
}

TestStatus test_all(const size_t s_tests) {
  TestStatus global_status = TEST_PASS;

  TestCase cases[] = {
    {"algo_queue && algo_pick", test_algo_queue_and_algo_pick, NULL, NULL},
    {"algo_a_star_best", test_algo_a_star_best, NULL, NULL},
    {"algo_search", test_algo_search, NULL, NULL},
    {"algo_update_tree", test_algo_update_tree, NULL, NULL},
  };

  TestResult results[sizeof(cases) / sizeof(cases[0])] = {0};
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    add_result(&cases[i], &results[i], s_tests);

    if (global_status == TEST_PASS)
      global_status = results[i].status;

    print_result(&results[i], s_tests);
  }

  return global_status;
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    panic(inval_arg, "argc < 2 (expected ./test_algo <tests>)");
  if (!argv[1])
    panic(nullptr, "argc < 2 (expected ./test_algo <tests>)");

  const size_t s_tests = (size_t)(str_to_int(argv[1]));
      
  // const size_t s_tests = 1000;
  if (!s_tests) {
    char buffer[str_len(argv[1]) + 1]; 
    snprintf(buffer, sizeof(buffer), "%s (expected ./test_type <tests>)\n", argv[1]);
    panic(inval_arg, buffer);
  }

  srand(time(NULL));
  int result = test_all(s_tests);
  print_exit_value(result);
  return result;
}
