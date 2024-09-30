#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "type.h"
#include "class_distribution.h"

typedef unsigned uint;

typedef struct {
  size_t max_students_per_classroom;
  size_t max_teachers_per_classroom;
  size_t max_same_class_per_week_per_student;
  size_t max_classes_per_classroom;
  size_t max_blocks_per_day_per_student;
  bool morning_and_afternoon;
} Rules;

typedef struct {
  size_t max_classes,
         min_classes,
         s_classes;
} ClassInfo;

typedef struct {
  size_t max_avail,
         min_avail; 
} AvailInfo;

#define check_error(condition, error) \
      do { \
        if (condition) { \
          fprintf(stderr, "[Error]: %s at %s (%s: %u)\n", #condition, __func__, __FILE__, __LINE__); \
          return error; \
        } \
      } while (0); \

double pow_int(double x, int power) {
  if (!x)
    return 0;
  if (!power)
    return 1;
  if (power == 1)
    return x;

  double r = x, y = 1;
  if (power < 0) {
    power *= -1;
    r = 1/r;
  }

  for (int i = 0; i < power; i++)
    y *= r;

  return y;
}

bool in_array(uint value, uint *array, size_t s_array) {
  if (!s_array)
    return false;
  for (size_t i = 0; i < s_array; i++)
    if (array[i] == value)
      return true;
  return false;
}

bool existing_entry(Entry *entries, size_t s_entries, size_t i, size_t j) {
  if (!entries)
    return false;

  for (size_t k = 0; k < s_entries; k++)
    if (entries[k].i == i && entries[k].j == j)
      return true;

  return false;
}

bool is_digit(char ch) {
  return (bool)(ch >= '0' && ch <= '9');
}

int strtoint(char *str) {
  int number = 0;
  for (; str && *str != '\0'; str++) {
    if (!is_digit(*str))
      return -1;
    number = number * 10 + *str - '0';
  }
  return number;
}

uint generate_id(uint status, uint len_id) {
  assert(inval_arg, len_id >= 5);
  return len_id <= 1 ? 0 : (status * pow_int(10, len_id - 1) + 24 * pow_int(10, len_id - 3) + rand()%((int)pow_int(10, len_id - 3)));
}

size_t find_class_placements(Assignment **placements, TimeTable *timetable, ID class) {
  assert(nullptr, placements != NULL);
  assert(nullptr, timetable != NULL);

  size_t s_temp = timetable->n * timetable->m / 2,
         s_placements = 0;
  *placements = (Assignment *)malloc(s_temp * sizeof(Assignment));
  assert(nullptr, *placements != NULL);
  
  assert(nullptr, timetable->entries != NULL);
  for (size_t i = 0; i < timetable->n; i++) {
    assert(nullptr, timetable->entries[i] != NULL);

    for (size_t j = 0; j < timetable->m; j++) {
      size_t s_classrooms = timetable->entries[i][j].s_classrooms;
      ClassRoom *classrooms = timetable->entries[i][j].classrooms;
      assert(nullptr, classrooms != NULL);

      for (size_t k = 0; k < s_classrooms; k++) {
        size_t s_classes = classrooms[k].s_classes;
        Class *classes = classrooms[k].classes;
        if (classes == NULL)
          continue;

        for (size_t l = 0; l < s_classes; l++) {
          if (classes[l].id != class)
            continue;

          if (s_placements >= s_temp) {
            s_temp *= 2;
            Assignment *temp = (Assignment *)realloc(*placements, s_temp * sizeof(Assignment));
            assert(nullptr, temp != NULL);
            *placements = temp;
          }

          (*placements)[s_placements].block.i = i;
          (*placements)[s_placements].block.j = j;
          (*placements)[s_placements].classroom = classrooms[k].id;
          (*placements)[s_placements++].class = classes[l].id;
        }
      }
    }
  }
  
  if (s_placements == 0) {
    if (*placements)
      free(*placements);
    *placements = NULL;
    return 0;
  }
  if (s_placements != s_temp) {
    Assignment *temp = (Assignment *)realloc(*placements, s_placements * sizeof(Assignment));
    assert(nullptr, temp != NULL);
    *placements = temp;
  }

  return s_placements;
}

size_t create_users(
  User **users, const ID *classes, const ClassInfo *class_info, const AvailInfo *avail_info,
  const size_t blocks_per_day, const size_t s_students, const size_t s_teachers
) {
  assert(nullptr, users != NULL);
  assert(nullptr, classes != NULL);
  assert(nullptr, class_info != NULL);
  assert(nullptr, avail_info != NULL);
  assert(inval_arg, blocks_per_day > 1);
  assert(inval_arg, s_students != 0);
  assert(inval_arg, s_teachers != 0);
  
  *users = (User *)malloc((s_students + s_teachers) * sizeof(User));
  assert(nullptr, *users != NULL);

  for (size_t i = 0; i < s_students + s_teachers; i++) {
    (*users)[i].type = i < s_students ? student_type : teacher_type;

    bool in_users = false;
    ID id;
    do {
      id = generate_id(i < s_students ? student_type : teacher_type, 6);
      for (size_t j = 0; j < i; j++)
        if (type_get_id(&(*users)[i]) == id) {
          in_users = true;
          break;
        }
    } while (in_users);
    type_set_id(&(*users)[i], id);

    size_t s_user_classes = class_info->min_classes; // rand() % (class_info->s_classes - class_info->min_classes + 1) + class_info->min_classes;
    ID *user_classes = (ID *)calloc(s_user_classes , sizeof(ID));
    assert(nullptr, user_classes != NULL);

    for (size_t k = 0; k < s_user_classes; k++) {
      uint value;
      do {
         value = classes[rand() % class_info->s_classes];
      } while(in_array(value, user_classes, s_user_classes));
      user_classes[k] = value;
    }

    type_set_size_classes(&(*users)[i], s_user_classes);
    type_set_classes(&(*users)[i], user_classes);

    size_t s_user_avail = rand() % (avail_info->max_avail - avail_info->min_avail + 1) + avail_info->min_avail,
           days_avail = 2 * s_user_avail / blocks_per_day;
    if (!days_avail)
      days_avail = 1;

    Entry *user_avail = (Entry *)calloc(s_user_avail, sizeof(Entry));
    assert(nullptr, user_avail != NULL);

    for (size_t j = 0; j < days_avail; j++) {
      bool morning = rand() % 2;
      size_t half_day = blocks_per_day / 2;
      for (size_t k = 0; k < half_day; k++) {
        user_avail[j * half_day + k].i = morning ? k : (blocks_per_day / 2 + k);
        user_avail[j * half_day + k].j = j;
      }
    }

    type_set_size_avail(&(*users)[i], s_user_avail);
    type_set_avail(&(*users)[i], user_avail);
  }

  return s_students + s_teachers;
}

TimeBlock **create_entries(size_t n, size_t m, size_t s_classrooms) {
  assert(inval_arg, n > 0);
  assert(inval_arg, m > 0);
  assert(inval_arg, s_classrooms > 0);

  TimeBlock **entries = (TimeBlock **)malloc(n * sizeof(TimeBlock *));
  assert(nullptr, entries != NULL);

  for (size_t i = 0; i < n; i++) {
    TimeBlock *timeblocks = (TimeBlock *)malloc(m * sizeof(TimeBlock));
    assert(nullptr, timeblocks != NULL);

    entries[i] = timeblocks;
    for (size_t j = 0; j < m; j++) {
      timeblocks[j].entry.i = i;
      timeblocks[j].entry.j = j;
      timeblocks[j].s_classrooms = s_classrooms;
      
      ClassRoom *classrooms = (ClassRoom *)malloc(s_classrooms * sizeof(ClassRoom));
      assert(nullptr, classrooms != NULL);

      timeblocks[j].classrooms = classrooms;
      for (size_t k = 0; k < s_classrooms; k++) {
        classrooms[k].id = k + 1;
        classrooms[k].s_classes = 0;
        classrooms[k].classes = NULL;
      }
    }
  }

  return entries;
}

int write_class(FILE *file, TimeTable *timetable, ID id) {
  check_error(file == NULL, 1);

  Assignment *placements = NULL;
  size_t s_placements = find_class_placements(&placements, timetable, id);
  fprintf(file, "\t%u\n", id);

  fprintf(file, "\t\t%zu\n", s_placements);
  for (size_t i = 0; i < s_placements; i++)
    fprintf(file, "\t\t\t%zu %zu %u\n", placements[i].block.i, placements[i].block.j, placements[i].classroom);

  if (placements)
    free(placements);

  return 0;
}

int write_user(FILE *file, User *user) {
  check_error(file == NULL, 1);
  check_error(user == NULL, 2);

  fprintf(file, "\t%u %u\n", type_get_is_student(user) ? student_type : teacher_type, type_get_id(user));

  size_t s_classes = type_get_size_classes(user);
  ID *classes = type_get_classes(user);
  
  fprintf(file, "\t\t%zu\n", s_classes);
  for (size_t i = 0; i < s_classes; i++)
    fprintf(file, "\t\t\t%u\n", classes[i]);
 
  size_t s_avail = type_get_size_avail(user);
  Entry *avail = type_get_avail(user);

  fprintf(file, "\t\t%zu\n", s_avail);
  for (size_t i = 0; i < s_avail; i++)
    fprintf(file, "\t\t\t%zu %zu\n", avail[i].i, avail[i].j);

  return 0;
}

int generate_mafi_read_file(
  FILE *file, const char *depth_str, const Rules *rules, const Entry *week_matrix,
  const size_t s_classrooms, const ClassInfo *class_info, const AvailInfo *avail_info,
  const size_t s_students, const size_t s_teachers
) {
  check_error(file == NULL, 1);
  check_error(depth_str == NULL, 2);
  check_error(rules == NULL, 3);
  check_error(week_matrix == NULL, 4);
  check_error(class_info == NULL, 5);
  check_error(avail_info == NULL, 6);

  fprintf(file, "%s\n\n", depth_str);

  srand(time(NULL));
  fprintf(
    file, "%zu %zu %zu %zu %zu %u\n\n",
    rules->max_students_per_classroom,
    rules->max_teachers_per_classroom,
    rules->max_same_class_per_week_per_student,
    rules->max_classes_per_classroom,
    rules->max_blocks_per_day_per_student,
    (uint)rules->morning_and_afternoon
  );

  fprintf(file, "%zu %zu\n", week_matrix->j, week_matrix->i);
  fprintf(file, "%zu\n", s_classrooms);
  for (size_t i = 0; i < s_classrooms; i++)
    fprintf(file, "\t%u\n", (uint)(i+1));
  fprintf(file, "\n");

  ID *classes = (ID *)malloc(class_info->s_classes * sizeof(ID));
  check_error(classes == NULL, 7);
  for (size_t i = 0; i < class_info->s_classes; i++)
    classes[i] = i + 1;

  User *users = NULL;
  size_t s_users = create_users(&users, classes, class_info, avail_info, week_matrix->i, s_students, s_teachers);

  // for (size_t i = 0; i < s_users; i++)
    // type_print_user(stdout, &users[i]);

  TimeBlock **entries = create_entries(week_matrix->i, week_matrix->j, s_classrooms);
  assert(nullptr, entries != NULL);
  TimeTable *timetable = type_create_timetable(entries, week_matrix->i, week_matrix->j);
  assert(nullptr, timetable != NULL);
  class_distribution(
    timetable, users, classes, s_users, class_info->s_classes, rules->max_classes_per_classroom, rules->max_students_per_classroom
  );
  // type_print_timetable(stdout, timetable);

  fprintf(file, "%zu\n", class_info->s_classes);
  for (size_t i = 0; i < class_info->s_classes; i++) {
    int error = write_class(file, timetable, classes[i]);
    check_error(error != 0, error);
  }
  fprintf(file, "\n");

  fprintf(file, "%zu\n", s_users);
  for (size_t i = 0; i < s_users; i++) {
    int error = write_user(file, &users[i]);
    check_error(error != 0, error);
  }

  free(classes);
  for (size_t i = 0; i < s_users; i++) {
    free(type_get_classes(&users[i]));
    free(type_get_avail(&users[i]));
  }
  free(users);
  type_delete_timetable(&timetable);

  return 0;
}

int main(int argc, char *argv[]) {
  check_error(argc < 9, 1);

  FILE *file = fopen(argv[1], "w");
  check_error(file == NULL, errno);

  const Entry week_matrix = { strtoint(argv[3]), strtoint(argv[4]) }; 

  const Rules rules = {
    .max_students_per_classroom = 10,
    .max_teachers_per_classroom = 3,
    .max_same_class_per_week_per_student = 1,
    .max_classes_per_classroom = 5,
    .max_blocks_per_day_per_student = week_matrix.i / 2,
    .morning_and_afternoon = false
  };

  const AvailInfo avail_info = {
    .max_avail = week_matrix.j * week_matrix.i / 2,
    .min_avail = avail_info.max_avail
  };

  const ClassInfo class_info = {
    .max_classes = strtoint(argv[6]),
    .min_classes = class_info.max_classes / 2,
    .s_classes = class_info.max_classes // rand() % (class_info.max_classes - class_info.min_classes + 1) + class_info.min_classes
  };

  const size_t s_classrooms = strtoint(argv[5]),
               s_students = strtoint(argv[7]),
               s_teachers = strtoint(argv[8]);

  int error = generate_mafi_read_file(file, argv[2], &rules, &week_matrix, s_classrooms, &class_info, &avail_info, s_students, s_teachers);

  if (file)
    fclose(file);

  return error;
}
