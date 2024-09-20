#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

typedef enum {
  false,
  true
} bool;

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
  size_t i, j;
} Entry;

typedef struct {
  size_t max_classes,
         min_classes,
         number_classes;
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

uint generate_id(uint status, uint len_id) {
  return len_id <= 1 ? 0 : (status * (int)pow_int(10, (int)len_id - 1) + rand()%((int)pow_int(10, (int)len_id - 1)));
}

int write_class(FILE *file, const size_t days, const size_t blocks_per_day, const uint number_classrooms, uint id) {
  check_error(file == NULL, 1);

  fprintf(file, "\t%u\n", id);

  size_t s_locations  = days * (rand() % ((size_t)(blocks_per_day/2)) + 1),
         k = 0;
  Entry *locations = (Entry *)malloc(s_locations * sizeof(Entry));
  check_error(locations == NULL, 2);

  fprintf(file, "\t\t%zu\n", s_locations);
  for (size_t j = 0; j < s_locations; j++) {
    size_t n, m;
    do {
      n = rand() % blocks_per_day;
      m = rand() % days;
    } while(existing_entry(locations, k, n, m));

    locations[k].i = n;
    locations[k++].j = m;

    fprintf(file, "\t\t\t%zu %zu %u\n", n, m, (uint)(rand() % number_classrooms + 1));
  }

  free(locations);

  return 0;
}

int write_user(FILE *file, const ClassInfo *class_info, const AvailInfo *avail_info, const size_t blocks_per_day, uint status, uint id) {
  check_error(file == NULL, 1);
  check_error(class_info == NULL, 2);
  check_error(avail_info == NULL, 3);
  check_error(blocks_per_day < 2, 4);

  fprintf(file, "\t%u %s%u\n", status, !status ? "0" : "", id);

  size_t s_classes = rand() % (class_info->number_classes - class_info->min_classes + 1) + class_info->min_classes,
         k = 0;
  uint *classes = (uint *)malloc(s_classes * sizeof(uint));
  check_error(classes == NULL, 5);

  fprintf(file, "\t\t%zu\n", s_classes);
  for (size_t i = 0; i < s_classes; i++) {
    uint value;
    do {
       value = rand() % (class_info->number_classes + 1);
    } while(in_array(value, classes, k));
    classes[k++] = value;

    fprintf(file, "\t\t\t%u\n", value);
  }

  free(classes);

  size_t user_avail = rand() % (avail_info->max_avail - avail_info->min_avail + 1) + avail_info->min_avail,
         days_avail = user_avail / 2;
  if (!days_avail)
    days_avail = 1;

  fprintf(file, "\t\t%zu\n", days_avail * blocks_per_day / 2);
  for (size_t j = 0; j < days_avail; j++) {
    bool morning = rand() % 2;
    for (size_t k = 0; k < blocks_per_day / 2; k++)
      fprintf(file, "\t\t\t%zu %zu\n", morning ? k : (blocks_per_day / 2 + k), j);
  }

  return 0;
}

int generate_mafi_read_file(FILE *file, char *depth_str) {
  check_error(file == NULL, 1);
  check_error(depth_str == NULL, 2);

  fprintf(file, "%s\n\n", depth_str);

  srand(time(NULL));
  const Rules rules = {
    .max_students_per_classroom = rand() % 2 ? 15 : 25,
    .max_teachers_per_classroom = rand() % 2 ? 2 : 3,
    .max_same_class_per_week_per_student = 1,
    .max_classes_per_classroom = rand() % 2 ? 2 : 3,
    .max_blocks_per_day_per_student = 2,
    .morning_and_afternoon = false
  };

  fprintf(
    file, "%zu %zu %zu %zu %zu %u\n\n",
    rules.max_students_per_classroom,
    rules.max_teachers_per_classroom,
    rules.max_same_class_per_week_per_student,
    rules.max_classes_per_classroom,
    rules.max_blocks_per_day_per_student,
    (uint)rules.morning_and_afternoon
  );

  const size_t days = 5, blocks_per_day = 4, number_classrooms = 3;
  fprintf(file, "%zu %zu\n\n", days, blocks_per_day);
  fprintf(file, "%zu\n", number_classrooms);
  for (size_t i = 0; i < number_classrooms; i++)
    fprintf(file, "\t%u\n", (uint)(i+1));
  fprintf(file, "\n");

  const ClassInfo class_info = {
    .max_classes = 5,
    .min_classes = 3,
    .number_classes = rand() % (class_info.max_classes - class_info.min_classes + 1) + class_info.min_classes
  };

  fprintf(file, "%zu\n", class_info.number_classes);
  for (size_t i = 0; i < class_info.number_classes; i++) {
    int error = write_class(file, days, blocks_per_day, number_classrooms, (uint)(i+1));
    check_error(error != 0, error);
  }
  fprintf(file, "\n");

  const size_t max_users = 10, min_users = 5,
               max_teachers = 3, min_teachers = 1;

  size_t s_users = rand() % (max_users - min_users + 1) + min_users,
         s_teachers = rand() % (max_teachers - min_teachers + 1) + min_teachers,
         s_students = s_users - s_teachers; 

  const AvailInfo avail_info = {
    .max_avail = days * blocks_per_day / 2,
    .min_avail = avail_info.max_avail / 4
  };

  fprintf(file, "%zu\n", s_users);

  for (size_t i = 0; i < s_students; i++) {
    int error = write_user(file, &class_info, &avail_info, blocks_per_day, 1, generate_id(1, 6));
    check_error(error != 0, error);
  }

  for (size_t i = 0; i < s_teachers; i++) {
    int error = write_user(file, &class_info, &avail_info, blocks_per_day, 2, generate_id(2, 6));
    check_error(error != 0, error);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  check_error(argc < 3, 1);

  FILE *file = fopen(argv[1], "w");
  check_error(file == NULL, errno);

  int error = generate_mafi_read_file(file, argv[2]);

  if (file)
    fclose(file);

  return error;
}
