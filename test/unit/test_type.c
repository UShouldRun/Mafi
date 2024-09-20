#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "testing.h"
#include "error.h"

#include "type.h"

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

bool in_array(uint x, uint *array, size_t len) {
  if (!array)
    return false;

  for (size_t i = 0; i < len; i++)
    if (array[i] == x)
      return true;

  return false;
}

int arrayset(uint *array, uint value, size_t len) {
  if (!array)
    return 1;
  if (!len)
    return 2;
  for (size_t i = 0; i < len; i++)
    array[i] = value;
  return 0;
}

#define check_fail(error, condition) \
          do { \
            if (!(condition)) break; \
            _print_error(error, #condition, __FILE__, __LINE__); \
            status = TEST_FAIL; \
            goto _return; \
          } while (0) \

#define check_error(error, condition) \
          do { \
            if (!(condition)) break; \
            _print_error(error, #condition, __FILE__, __LINE__); \
            status = TEST_ERROR; \
            goto _return; \
          } while (0) \

TestStatus tt_create_class_and_delete_class() {
  TestStatus status = TEST_PASS;

  ID id = rand() % 100;

  size_t s_students = rand() % 50 + 20,
         s_teachers = rand() % 15 + 5;

  ID *students = (ID *)malloc(s_students * sizeof(ID));
  check_error(mem_error, !students);
  ID *teachers = (ID *)malloc(s_teachers * sizeof(ID));;
  check_error(mem_error, !teachers);

  Class *class = type_create_class(id, students, teachers, s_students, s_teachers);
  check_fail(nullptr, !class);

  check_fail(unexp_behaviour, class->id != id);
  check_fail(unexp_behaviour, class->students != students);
  check_fail(unexp_behaviour, class->teachers != teachers);
  check_fail(unexp_behaviour, class->len_students != s_students);
  check_fail(unexp_behaviour, class->len_teachers != s_teachers);

  type_delete_class(&class, 1);
  check_fail(unexp_behaviour, class);

_return:
  return status;
}

TestStatus tt_create_classroom_and_delete_classroom() {
  TestStatus status = TEST_PASS;

  ID id = rand() % 100;
  ClassRoom *classroom = type_create_classroom(id, NULL, 0);
  check_fail(nullptr, !classroom);

  check_fail(unexp_behaviour, classroom->id != id);

  type_delete_classroom(&classroom, 1);
  check_fail(unexp_behaviour, classroom);
  
_return:
  return status;
}

TestStatus tt_create_timeblock_and_delete_timeblock() {
  TestStatus status = TEST_PASS;

  Entry entry = { .i = rand() % 5, .j = rand() % 6 };
  TimeBlock *timeblock = type_create_timeblock(&entry, NULL, 0);
  check_fail(nullptr, !timeblock);

  check_fail(unexp_behaviour, timeblock->entry.i != entry.i || timeblock->entry.j != entry.j);

  type_delete_timeblock(&timeblock, 1);
  check_error(unexp_behaviour, timeblock);

_return:
  return status;
}

TestStatus tt_create_timetable_and_delete_timetable() {
  TestStatus status = TEST_PASS;

  size_t n = 4, m = 5;
  TimeBlock **entries = (TimeBlock **)malloc(n * sizeof(TimeBlock *));
  check_error(mem_error, !entries);
  for (size_t i = 0; i < n; i++) {
    entries[i] = (TimeBlock *)malloc(m * sizeof(TimeBlock));
    check_error(mem_error, !entries[i]);

    Entry entry = { .i = i, .j = 0 };
    for (size_t j = 0; j < m; j++) {
      entry.j = j;
      TimeBlock *block = type_create_timeblock(&entry, NULL, 0);
      check_error(nullptr, !block);
      entries[i][j] = *block;
    }
  }

  TimeTable *timetable = type_create_timetable(entries, n, m);
  check_fail(mem_error, !timetable);
  check_fail(unexp_behaviour, timetable->entries != entries);
  check_fail(unexp_behaviour, timetable->n != n || timetable->m != m);

  type_delete_timetable(&timetable);
  check_fail(unexp_behaviour, timetable);

_return:
  return status;
}

TestStatus tt_copy_user_ids() {
  TestStatus status = TEST_PASS;

  size_t s_ids = rand() % 50 + 20;
  ID *ids = (ID *)malloc(s_ids * sizeof(ID));
  check_error(mem_error, !ids);

  for (size_t i = 0; i < s_ids; i++)
    ids[i] = i;

  ID *cp = type_copy_user_ids(ids, s_ids);
  check_fail(nullptr, !cp);

  for (size_t i = 0; i < s_ids; i++)
    check_fail(unexp_behaviour, cp[i] != ids[i]);

_return:
  if (cp)
    free(cp);
  if (ids)
    free(ids);
  return status;
}

TestStatus tt_copy_classes() {
  TestStatus status = TEST_PASS;

  size_t s_classes = rand() % 20 + 5;
  Class *classes = (Class *)malloc(s_classes * sizeof(Class));
  check_error(mem_error, !classes);

  size_t *sizes_students = (size_t *)malloc(s_classes * sizeof(size_t));
  check_error(mem_error, !sizes_students);

  ID **students = (ID **)malloc(s_classes * sizeof(ID *));
  check_error(mem_error, !students);

  size_t *sizes_teachers = (size_t *)malloc(s_classes * sizeof(size_t));
  check_error(mem_error, !sizes_teachers);

  ID **teachers = (ID **)malloc(s_classes * sizeof(ID *));
  check_error(mem_error, !teachers);


  for (size_t i = 0; i < s_classes; i++) {
    classes[i].id = rand() % 100;

    sizes_students[i] = rand() % 6 + 5;
    sizes_teachers[i] = rand() % 5 + 1;

    students[i] = (ID *)malloc(sizes_students[i] * sizeof(ID));
    check_error(mem_error, !students[i]);
    for (size_t j = 0; j < sizes_students[i]; j++)
      students[i][j] = j;

    teachers[i] = (ID *)malloc(sizes_teachers[i] * sizeof(ID));
    check_error(mem_error, !teachers[i]);
    for (size_t j = 0; j < sizes_teachers[i]; j++)
      teachers[i][j] = j;

    classes[i].len_students = sizes_students[i];
    classes[i].students = students[i];

    classes[i].len_teachers = sizes_teachers[i];
    classes[i].teachers = teachers[i];
  }

  Class *cp = type_copy_classes(classes, s_classes);
  check_fail(nullptr, !cp);

  for (size_t i = 0; i < s_classes; i++) {
    check_fail(unexp_behaviour, cp[i].id != classes[i].id);

    check_fail(unexp_behaviour, cp[i].len_students != classes[i].len_students);
    check_fail(unexp_behaviour, cp[i].len_teachers != classes[i].len_teachers);

    for (size_t j = 0; j < sizes_students[i]; j++)
      check_fail(unexp_behaviour, cp[i].students[j] != classes[i].students[j]);
    for (size_t j = 0; j < sizes_teachers[i]; j++)
      check_fail(unexp_behaviour, cp[i].teachers[j] != classes[i].teachers[j]);
  }
 
_return:
  if (cp) {
    for (size_t i = 0; i < s_classes; i++) {
      free(cp[i].students);
      free(cp[i].teachers);
    }
    free(cp);
  }
  if (classes) {
    for (size_t i = 0; i < s_classes; i++) {
      free(classes[i].students);
      free(classes[i].teachers);
    }
    free(classes);
  }
  if (sizes_students)
    free(sizes_students);
  if (sizes_teachers)
    free(sizes_teachers);
  if (students)
    free(students);
  if (teachers)
    free(teachers);
  return status;
}

TestStatus tt_copy_classrooms() {
  TestStatus status = TEST_PASS;

  size_t s_classrooms = rand() % 25 + 5;
  ClassRoom *classrooms = (ClassRoom *)malloc(s_classrooms * sizeof(ClassRoom));
  check_error(mem_error, !classrooms);

  for (size_t i = 0; i < s_classrooms; i++) {
    classrooms[i].id = rand() % 100;
    classrooms[i].len = rand() % 3 + 1;

    Class *classes = (Class *)malloc(classrooms[i].len * sizeof(Class));
    check_error(mem_error, !classes);

    for (size_t j = 0; j < classrooms[i].len; j++) {
      size_t s_students = rand() % 10 + 1,
             s_teachers = rand() % 5 + 1;

      ID *students = (ID *)malloc(s_students * sizeof(ID));
      check_error(mem_error, !students);

      ID *teachers = (ID *)malloc(s_teachers * sizeof(ID));
      check_error(mem_error, !teachers);

      for (size_t k = 0; k < s_students; k++)
        students[k] = k;
      for (size_t k = 0; k < s_teachers; k++)
        teachers[k] = k;

      classes[j].id = j;
      classes[j].len_students = s_students;
      classes[j].len_teachers = s_teachers;
      classes[j].students = students;
      classes[j].teachers = teachers;
    }

    classrooms[i].classes = classes;
  }

  ClassRoom *cp = type_copy_classrooms(classrooms, s_classrooms);
  check_fail(nullptr, !cp);

  for (size_t i = 0; i < s_classrooms; i++) {
    check_fail(unexp_behaviour, cp[i].id != classrooms[i].id);
    check_fail(unexp_behaviour, cp[i].len != classrooms[i].len);
    check_fail(nullptr, !cp[i].classes);

    for (size_t j = 0; j < classrooms[i].len; j++) {
      check_fail(unexp_behaviour, cp[i].classes[j].id != classrooms[i].classes[j].id);
      check_fail(nullptr, !cp[i].classes[j].students);
      check_fail(nullptr, !cp[i].classes[j].teachers);

      for (size_t k = 0; k < classrooms[i].classes[j].len_students; k++)
        check_fail(unexp_behaviour, cp[i].classes[j].students[k] != classrooms[i].classes[j].students[k]);
      for (size_t k = 0; k < classrooms[i].classes[j].len_teachers; k++)
        check_fail(unexp_behaviour, cp[i].classes[j].teachers[k] != classrooms[i].classes[j].teachers[k]);
    }
  }

_return:
  if (cp) {
    for (size_t i = 0; i < s_classrooms; i++) {
      for (size_t j = 0; j < cp[i].len; j++) {
        free(cp[i].classes[j].students);
        free(cp[i].classes[j].teachers);
      }
      free(cp[i].classes);
    }
    free(cp);
  }
  if (classrooms) {
    for (size_t i = 0; i < s_classrooms; i++) {
      for (size_t j = 0; j < classrooms[i].len; j++) {
        free(classrooms[i].classes[j].students);
        free(classrooms[i].classes[j].teachers);
      }
      free(classrooms[i].classes);
    }
    free(classrooms);
  }
  return status;
}

TestStatus tt_copy_timeblocks() {
  TestStatus status = TEST_PASS;

  size_t s_timeblocks = rand() % 5 + 1;
  TimeBlock *timeblocks = (TimeBlock *)malloc(s_timeblocks * sizeof(TimeBlock));
  check_error(mem_error, !timeblocks);

  size_t j = rand() % 5;
  for (size_t i = 0; i < s_timeblocks; i++) {
    timeblocks[i].entry.i = i;
    timeblocks[i].entry.j = j;

    timeblocks[i].len = rand() % 5 + 1;
    ClassRoom *classrooms = (ClassRoom *)malloc(timeblocks[i].len * sizeof(ClassRoom));
    check_error(mem_error, !classrooms);

    for (size_t j = 0; j < timeblocks[i].len; j++) {
      classrooms[j].id = j;
      classrooms[j].len = rand() % 5 + 1;

      Class *classes = (Class *)malloc(classrooms[j].len * sizeof(Class));
      check_error(mem_error, !classes);
      for (size_t k = 0; k < classrooms[j].len; k++) {
        classes[k].id = k;
        classes[k].len_students = rand() % 6 + 5;
        classes[k].len_teachers = rand() % 3 + 1;

        ID *students = (ID *)malloc(classes[k].len_students * sizeof(ID));
        check_error(mem_error, !students);

        ID *teachers = (ID *)malloc(classes[k].len_teachers * sizeof(ID));
        check_error(mem_error, !teachers);

        for (size_t l = 0; l < classes[k].len_students; l++)
          students[l] = l;
        for (size_t l = 0; l < classes[k].len_teachers; l++)
          teachers[l] = l;

        classes[k].students = students;
        classes[k].teachers = teachers;
      }

      classrooms[j].classes = classes;
    }

    timeblocks[i].classrooms = classrooms;
  }

  TimeBlock *cp = type_copy_timeblocks(timeblocks, s_timeblocks);
  check_fail(nullptr, !cp);

  for (size_t i = 0; i < s_timeblocks; i++) {
    check_fail(unexp_behaviour, cp[i].entry.i != timeblocks[i].entry.i || cp[i].entry.j != timeblocks[i].entry.j);
    check_fail(unexp_behaviour, cp[i].len != timeblocks[i].len);
    check_fail(unexp_behaviour, !cp[i].classrooms);

    size_t s_classrooms = timeblocks[i].len;
    ClassRoom *t_classrooms = timeblocks[i].classrooms,
              *cp_classrooms = cp[i].classrooms;

    for (size_t j = 0; j < s_classrooms; j++) {
      check_fail(unexp_behaviour, cp_classrooms[j].id != t_classrooms[j].id);
      check_fail(unexp_behaviour, cp_classrooms[j].len != t_classrooms[j].len);
      check_fail(nullptr, !cp_classrooms[j].classes);

      size_t s_classes = t_classrooms[j].len;
      Class *t_classes = t_classrooms[j].classes,
              *cp_classes = cp_classrooms[j].classes;

      for (size_t k = 0; k < s_classes; k++) {
        check_fail(unexp_behaviour, cp_classes[k].id != t_classes[k].id);
        check_fail(unexp_behaviour, cp_classes[k].len_students != t_classes[k].len_students);
        check_fail(unexp_behaviour, cp_classes[k].len_teachers != t_classes[k].len_teachers);
        check_fail(nullptr, !cp_classes[k].students);
        check_fail(nullptr, !cp_classes[k].teachers);

        size_t s_students = cp_classes[k].len_students,
               s_teachers = cp_classes[k].len_teachers;

        ID *cp_students = cp_classes[k].students,
           *t_students = t_classes[k].students,
           *cp_teachers = cp_classes[k].teachers,
           *t_teachers = t_classes[k].teachers;

        for (size_t l = 0; l < s_students; l++)
          check_fail(unexp_behaviour, cp_students[l] != t_students[l]);
        for (size_t l = 0; l < s_teachers; l++)
          check_fail(unexp_behaviour, cp_teachers[l] != t_teachers[l]);
      }
    }
  }

_return:
  if (cp) {
    for (size_t i = 0; i < s_timeblocks; i++) {
      for (size_t j = 0; j < cp[i].len; j++) {
        for (size_t k = 0; k < cp[i].classrooms[j].len; k++) {
          free(cp[i].classrooms[j].classes[k].students);
          free(cp[i].classrooms[j].classes[k].teachers);
        }
        free(cp[i].classrooms[j].classes);
      }
      free(cp[i].classrooms);
    }
    free(cp);
  }
  if (timeblocks) {
    for (size_t i = 0; i < s_timeblocks; i++) {
      for (size_t j = 0; j < timeblocks[i].len; j++) {
        for (size_t k = 0; k < timeblocks[i].classrooms[j].len; k++) {
          free(timeblocks[i].classrooms[j].classes[k].students);
          free(timeblocks[i].classrooms[j].classes[k].teachers);
        }
        free(timeblocks[i].classrooms[j].classes);
      }
      free(timeblocks[i].classrooms);
    }
    free(timeblocks);
  }
  return status;
}

TestStatus tt_get_assign_from_timetable_with_id() {
  TestStatus status = TEST_PASS;

  size_t s_students = rand() % 20 + 5;
  ID *students = (ID *)malloc(s_students * sizeof(ID));
  check_error(mem_error, !students);
  check_error(unexp_behaviour, arrayset(students, 0, s_students));

  for (size_t i = 0; i < s_students; i++) {
    ID id;
    do {
      id = rand() % 100 + 1;
    } while (in_array(id, students, s_students));
    students[i] = id;
  }

  size_t s_teachers = rand() % 10 + 5;
  ID *teachers = (ID *)malloc(s_teachers * sizeof(ID));
  check_error(mem_error, !teachers);
  check_error(unexp_behaviour, arrayset(teachers, 0, s_teachers));

  for (size_t i = 0; i < s_teachers; i++) {
    ID id;
    do {
      id = rand() % 100 + 1;
    } while (in_array(id, teachers, s_teachers));
    teachers[i] = id;
  }

  size_t s_classrooms = rand() % 5 + 4;
  ID *classrooms_id = (ID *)malloc(s_classrooms * sizeof(ID));
  check_error(mem_error, !classrooms_id);
  
  for (size_t i = 0; i < s_classrooms; i++)
    classrooms_id[i] = i;

  size_t s_classes = rand() % 5 + 4;
  ID *classes_id = (ID *)malloc(s_classes * sizeof(ID));
  check_error(mem_error, !classes_id);
  
  for (size_t i = 0; i < s_classes; i++)
    classes_id[i] = i;

  size_t n = 4, m = 5;
  TimeBlock **entries = (TimeBlock **)malloc(n * sizeof(TimeBlock *));
  check_error(mem_error, !entries);

  for (size_t i = 0; i < n; i++) {
    entries[i] = (TimeBlock *)malloc(m * sizeof(TimeBlock));
    check_error(mem_error, !entries[i]);

    for (size_t j = 0; j < m; j++) {
      entries[i][j].entry.i = i;
      entries[i][j].entry.j = j;
      
      ClassRoom *classrooms = (ClassRoom *)malloc(s_classrooms * sizeof(ClassRoom));
      check_error(mem_error, !classrooms);

      for (size_t k = 0; k < s_classrooms; k++) {
        classrooms[k].id = classrooms_id[k];

        size_t len = rand() % (s_classes / 2) + 1;
        Class *classes = (Class *)malloc(len * sizeof(Class));
        check_error(mem_error, !classes);

        for (size_t l = 0; l < len; l++) {
          classes[l].id = classes_id[l];

          size_t len_students = rand() % (s_students - 4) + 5,
                 len_teachers = rand() % (s_teachers - 2) + 3;

          ID *_students = (ID *)malloc(len_students * sizeof(ID));
          check_error(mem_error, !_students);
          check_error(unexp_behaviour, arrayset(_students, 0, len_students));

          for (size_t m = 0; m < len_students; m++) {
            ID x;
            do {
              x = students[rand() % s_students];
            } while (in_array(x, _students, len_students));
            _students[m] = x;
          }

          ID *_teachers = (ID *)malloc(len_teachers * sizeof(ID));
          check_error(mem_error, !_teachers);
          check_error(unexp_behaviour, arrayset(_teachers, 0, len_teachers));

          for (size_t m = 0; m < len_teachers; m++) {
            ID x;
            do {
              x = teachers[rand() % s_teachers];
            } while (in_array(x, _teachers, len_teachers));
            _teachers[m] = x;
          }

          classes[l].students = _students;
          classes[l].teachers = _teachers;
          classes[l].len_students = len_students;
          classes[l].len_teachers = len_teachers;
        }

        classrooms[k].classes = classes;
        classrooms[k].len = len;
      }

      entries[i][j].len = s_classrooms;
      entries[i][j].classrooms = classrooms;
    }
  }

  TimeTable *timetable = type_create_timetable(entries, n, m);
  check_error(nullptr, !timetable);

  ID user = students[rand() % s_students];

  Assignment *assign = NULL;
  size_t s_assign = type_get_assign_from_timetable_with_id(&assign, timetable, user);
  check_fail(nullptr, !assign);
  check_fail(unexp_behaviour, !s_assign);

  for (size_t i = 0; i < s_assign; i++) {
    TimeBlock timeblock = timetable->entries[assign[i].block.i][assign[i].block.j];
    check_error(nullptr, timeblock.classrooms == NULL);

    ClassRoom classroom;
    Class class;

    for (size_t j = 0; j < timeblock.len; j++)
      if (timeblock.classrooms[j].id == assign[i].classroom) {
        classroom = timeblock.classrooms[j];
        break;
      }

    check_error(nullptr, classroom.classes == NULL);

    for (size_t j = 0; j < classroom.len; j++)
      if (classroom.classes[j].id == assign[i].class) {
        class = classroom.classes[j];
        break;
      }

    check_error(nullptr, class.students == NULL);
    check_error(nullptr, class.teachers == NULL);

    bool is_in_class = false;
    for (size_t j = 0; j < class.len_students; j++)
      if (class.students[j] == user) {
        is_in_class++;
        break;
      }
    for (size_t j = 0; j < class.len_teachers; j++)
      if (class.teachers[j] == user) {
        is_in_class++;
        break;
      }

    check_fail(unexp_behaviour, !is_in_class);
  }

_return:
  type_delete_timetable(&timetable);
  check_error(unexp_behaviour, timetable);

  free(assign);

  free(students);
  free(teachers);
  free(classes_id);
  free(classrooms_id);

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
    {"type_create_class & type_delete_class", tt_create_class_and_delete_class, NULL, NULL},
    {"type_create_classroom & type_delete_classroom", tt_create_classroom_and_delete_classroom, NULL, NULL},
    {"type_create_timeblock & type_delete_timeblock", tt_create_timeblock_and_delete_timeblock, NULL, NULL},

    {"type_copy_user_ids", tt_copy_user_ids, NULL, NULL},
    {"type_copy_classes", tt_copy_classes, NULL, NULL},
    {"type_copy_classrooms", tt_copy_classrooms, NULL, NULL},
    {"type_copy_timeblocks", tt_copy_timeblocks, NULL, NULL},

    {"type_get_assign_from_timetable_with_id", tt_get_assign_from_timetable_with_id, NULL, NULL},
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
    panic(inval_arg, "argc < 2 (expected ./test_type <tests>)");
  if (!argv[1])
    panic(nullptr, "argv[1] is NULL (expected ./test_type <tests>)");

  const size_t s_tests = (size_t)str_to_int(argv[1]);
  
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
