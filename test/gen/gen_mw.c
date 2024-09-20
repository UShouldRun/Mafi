#include <time.h>

#include "type.h"
#include "error.h"

TimeTable *gen(size_t n, size_t m, size_t s_classrooms, size_t s_classes) {
  assert(inval_arg, s_classrooms != 0);
  assert(inval_arg, s_classes >= 2);

  TimeTable *timetable = (TimeTable *)malloc(sizeof(TimeTable));
  assert(nullptr, timetable != NULL);

  timetable->n = n;
  timetable->m = m;

  timetable->entries = (TimeBlock **)malloc(timetable->n * sizeof(TimeBlock *));
  assert(mem_error, timetable->entries != NULL);
  for (size_t i = 0; i < timetable->n; i++) {
    timetable->entries[i] = (TimeBlock *)malloc(timetable->m * sizeof(TimeBlock));
    assert(mem_error, timetable->entries[i] != NULL);

    for (size_t j = 0; j < timetable->m; j++) {
      timetable->entries[i][j].classrooms = (ClassRoom *)malloc(s_classrooms * sizeof(ClassRoom));
      assert(mem_error, timetable->entries[i][j].classrooms != NULL);
      timetable->entries[i][j].len = s_classrooms;
      timetable->entries[i][j].entry.i = i;
      timetable->entries[i][j].entry.j = j;

      ClassRoom *classrooms = timetable->entries[i][j].classrooms;
      for (size_t k = 0; k < s_classrooms; k++) {
        classrooms[k].id = k + 1;
        classrooms[k].len = rand() % 2 + 1;

        classrooms[k].classes = (Class *)malloc(classrooms[k].len * sizeof(Class));
        assert(mem_error, classrooms[k].classes != NULL);

        for (size_t l = 0; l < classrooms[k].len; l++) {
          classrooms[k].classes[l].id = rand() % s_classes + 1;
          classrooms[k].classes[l].len_students = 0;
          classrooms[k].classes[l].len_teachers = 0;

          classrooms[k].classes[l].students = NULL;
          classrooms[k].classes[l].teachers = NULL;
        }
      }
    }
  }

  return timetable;
}

bool in_array(int x, int vec[], size_t s_vec) {
  if (!vec)
    return false;
  if (!s_vec)
    return false;
  for (size_t i = 0; i < s_vec; i++)
    if (vec[i] == x)
      return true;
  return false;
}

void insert(int x, size_t pos, int vec[], size_t s_vec) {
  if (!vec)
    return;
  if (pos >= s_vec)
    return;
  vec[pos] = x;
}

void assign_user(TimeTable *timetable, ID *students, ID *teachers, size_t s_students, size_t s_teachers) {
  assert(nullptr, timetable != NULL);
  assert(nullptr, students != NULL);
  assert(nullptr, teachers != NULL);
  assert(inval_arg, s_students != 0);
  assert(inval_arg, s_teachers != 0);

  for (size_t k = 0; k < s_students; k++) {
    uint week = rand() % 32;

    int days[3] = { -1, -1, -1 };
    for (size_t j = 0; j < 3; j++) {
      int day;
      do {
        day = rand() % 5;
      } while (in_array(day, days, 3));
      insert(day, j, days, 3);


      for (size_t i = 0; i < timetable->n; i++) {
        bool not_morning_when_selected = (i >= timetable->n / 2) && !((week << day) % 2);
        bool not_afternoon_when_selected = (i < timetable->n / 2) && ((week << day) % 2);
        if (not_morning_when_selected || not_afternoon_when_selected)
          continue;

        size_t len_classrooms = timetable->entries[i][day].len;
        ClassRoom *classroom = &timetable->entries[i][day].classrooms[rand() % len_classrooms];
        Class *class = &classroom->classes[rand() % classroom->len];
        
        ID *temp = (ID *)realloc(class->students, (class->len_students + 1) * sizeof(ID));
        assert(mem_error, temp != NULL);
        class->students = temp;
        class->students[class->len_students++] = students[k];
      }
    }
  }
  
  for (size_t k = 0; k < s_teachers; k++) {
    int shift_type = rand() % 3;

    int days[5] = { -1, -1, -1, -1, -1 };
    size_t num_days = rand() % 5 + 1;

    for (size_t j = 0; j < num_days; j++) {
      int day;
      do {
        day = rand() % 5;
      } while (in_array(day, days, 5));
      insert(day, j, days, 5);

      for (size_t i = 0; i < timetable->n; i++) {
        bool is_morning = i < timetable->n / 2;
        bool is_afternoon = i >= timetable->n / 2;

        if ((shift_type == 1 && is_afternoon) || (shift_type == 2 && is_morning))
          continue;

        size_t len_classrooms = timetable->entries[i][day].len;
        ClassRoom *classroom = &timetable->entries[i][day].classrooms[rand() % len_classrooms];
        Class *class = &classroom->classes[rand() % classroom->len];

        ID *temp = (ID *)realloc(class->teachers, (class->len_teachers + 1) * sizeof(ID));
        assert(mem_error, temp != NULL);
        class->teachers = temp;
        class->teachers[class->len_teachers++] = teachers[k];
      }
    }
  }
}

void store(char *write_file_name, TimeTable **timetables, size_t s_timetables) {
  assert(nullptr, write_file_name != NULL);
  assert(nullptr, timetables != NULL);
  assert(inval_arg, s_timetables != 0);

  FILE *write_file = fopen(write_file_name, "w");
  assert(ioerror, write_file != NULL);

  fprintf(write_file, "%zu\n", s_timetables);
  for (size_t number = 0; number < s_timetables; number++) {
    TimeTable *timetable = timetables[number];
    fprintf(write_file, "%zu %zu\n", timetable->n, timetable->m);

    for (size_t i = 0; i < timetable->n; i++) {
      assert(nullptr, timetable->entries[i] != NULL);

      for (size_t j = 0; j < timetable->m; j++) {
        fprintf(write_file, "\t%zu %zu %zu\n", i, j, timetable->entries[i][j].len);
        ClassRoom *classrooms = timetable->entries[i][j].classrooms;
        assert(nullptr, classrooms != NULL);

        for (size_t k = 0; k < timetable->entries[i][j].len; k++) {
          fprintf(write_file, "\t\t%u %zu\n", classrooms[k].id, classrooms[k].len);
          Class *classes = classrooms[k].classes;
          assert(nullptr, classes != NULL);

          for (size_t l = 0; l < classrooms[k].len; l++) {
            fprintf(write_file, "\t\t\t%u %zu %zu\n", classes[l].id, classes[l].len_students, classes[l].len_teachers);
            ID *students = classes[l].students;
            ID *teachers = classes[l].teachers;

            if (students)
              for (size_t m = 0; m < classes[l].len_students; m++)
                fprintf(write_file, "\t\t\t\t%u\n", students[m]);

            fprintf(write_file, "\n");

            if (teachers)
              for (size_t m = 0; m < classes[l].len_teachers; m++)
                fprintf(write_file, "\t\t\t\t%u\n", teachers[m]);
          }
        }
      }
    }
  }

  fclose(write_file);
}

bool is_digit(char ch) {
  return (bool)(ch >= '0' && ch <= '9');
}

bool str_is_digit(char *str) {
  if (!str)
    return false;
  for (; *str != '\0'; str++)
    if (!is_digit(*str))
      return false;
  return true;
}

int strtoint(char *str) {
  if (!str)
    return 0;
  int number = 0;
  for (; *str != '\0'; str++)
    number = number * 10 + (*str - '0');
  return number;
}

int main(int argc, char *argv[]) {
  assert(inval_arg, argc >= 9);
  assert(nullptr, argv[1] != NULL);
  assert(inval_arg, str_is_digit(argv[2]) != false);
  assert(inval_arg, str_is_digit(argv[3]) != false);
  assert(inval_arg, str_is_digit(argv[4]) != false);
  assert(inval_arg, str_is_digit(argv[5]) != false);
  assert(inval_arg, str_is_digit(argv[6]) != false);
  assert(inval_arg, str_is_digit(argv[7]) != false);
  assert(inval_arg, str_is_digit(argv[8]) != false);

  srand(time(NULL));

  size_t s_timetables = (size_t)strtoint(argv[2]);
  TimeTable **timetables = (TimeTable **)malloc(s_timetables * sizeof(TimeTable *));

  const size_t n = (size_t)strtoint(argv[3]);
  const size_t m = (size_t)strtoint(argv[4]);

  const size_t s_classrooms = (size_t)strtoint(argv[5]);
  const size_t s_classes = (size_t)strtoint(argv[6]);

  const size_t s_students = (size_t)strtoint(argv[7]);
  ID students[s_students];
  for (size_t i = 0; i < s_students; i++)
    students[i] = 100000 + i;

  const size_t s_teachers = (size_t)strtoint(argv[8]);
  ID teachers[s_teachers];
  for (size_t i = 0; i < s_teachers; i++)
    teachers[i] = 200000 + i;

  for (size_t i = 0; i < s_timetables; i++) {
    timetables[i] = gen(n, m, s_classrooms, s_classes);
    assert(nullptr, timetables[i] != NULL);
    assign_user(timetables[i], students, teachers, s_students, s_teachers);
  }
  
  store(argv[1], timetables, s_timetables);

  for (size_t i = 0; i < s_timetables; i++)
    type_delete_timetable(&timetables[i]);
  free(timetables);

  return 0;
}
