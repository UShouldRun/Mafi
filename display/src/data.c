#include "data.h"

Error read_mw(FILE *file, TimeTable **timetables, size_t *s_timetables) {
  check_error(nullptr, file == NULL);
  check_error(nullptr, timetables == NULL);
  
  fscanf(file, "%zu\n", s_timetables);
  *timetables = (TimeTable *)malloc((*s_timetables) * sizeof(TimeTable));
  check_error(mem_error, *timetables == NULL);

  for (size_t t = 0; t < *s_timetables; t++) {
    TimeTable *timetable = &(*timetables)[t];
    fscanf(file, "%zu %zu\n", &timetable->n, &timetable->m);

    timetable->entries = (TimeBlock **)malloc(timetable->n * sizeof(TimeBlock *));
    check_error(mem_error, timetable->entries == NULL);

    for (size_t i = 0; i < timetable->n; i++) { 
      timetable->entries[i] = (TimeBlock *)malloc(timetable->m * sizeof(TimeBlock));
      check_error(mem_error, timetable->entries[i] == NULL);

      for (size_t j = 0; j < timetable->m; j++) {
        fscanf(
          file,
          "\t%zu %zu %zu\n",
          &timetable->entries[i][j].entry.i,
          &timetable->entries[i][j].entry.j,
          &timetable->entries[i][j].s_classrooms
        );

        size_t s_classrooms = timetable->entries[i][j].s_classrooms;
        ClassRoom *classrooms = (ClassRoom *)malloc(s_classrooms * sizeof(ClassRoom));
        check_error(mem_error, classrooms == NULL);
        timetable->entries[i][j].classrooms = classrooms;

        for (size_t k = 0; k < s_classrooms; k++) {
          fscanf(file, "\t\t%u %zu\n", &classrooms[k].id, &classrooms[k].s_classes);

          size_t s_classes = classrooms[k].s_classes;
          Class *classes = (Class *)malloc(s_classes * sizeof(Class));
          check_error(mem_error, classes == NULL);

          classrooms[k].classes = classes;
          for (size_t l = 0; l < s_classes; l++) {
            fscanf(
              file,
              "\t\t\t%u %zu %zu\n",
              &classes[l].id,
              &classes[l].s_students,
              &classes[l].s_teachers
            );

            size_t s_students = classes[l].s_students,
                   s_teachers = classes[l].s_teachers;

            if (s_students) {
              ID *students = (ID *)malloc(s_students * sizeof(ID));
              check_error(mem_error, students == NULL);

              for (size_t m = 0; m < s_students; m++)
                fscanf(file, "\t\t\t\t%u\n", &students[m]);

              classes[l].students = students;
            } else {
              classes[l].students = NULL;
            }
            
            fscanf(file, "\n");

            if (s_teachers) {
              ID *teachers = (ID *)malloc(s_teachers * sizeof(ID));
              check_error(mem_error, teachers == NULL);

              for (size_t m = 0; m < s_teachers; m++)
                fscanf(file, "\t\t\t\t%u\n", &teachers[m]);

              classes[l].teachers = teachers;
            } else {
              classes[l].teachers = NULL;
            }

            fscanf(file, "\n");
          }
          fscanf(file, "\n");
        }
        fscanf(file, "\n");
      }
      fscanf(file, "\n");
    } 
  }

  return none;
}

size_t append(uint data, uint **array, size_t s_array) {
  if (!array)
    return 0;

  ID *new_array = (uint *)realloc(*array, (s_array + 1) * sizeof(uint));
  if (!new_array)
    return 0;

  *array = new_array;
  (*array)[s_array++] = data;

  return s_array;
}

bool included(ID user, ID *users, size_t s_users) {
  if (!users)
    return false;

  for (size_t i = 0; i < s_users; i++)
    if (users[i] == user)
      return true;

  return false;
}

int in_schedule(Assignment *assign, size_t s_assign, size_t i, size_t j) {
  if (!assign)
    return -1;

  size_t k = 0;
  for (; k < s_assign; i++)
    if (assign[k].block.i == i && assign[k].block.j == j)
      return k;

  return -1;
}

size_t get_users(TimeTable *timetable, ID **users) {
  if (!timetable)
    return 0;
  assert(nullptr, timetable->entries != NULL);

  size_t s_users = 0;
  for (size_t i = 0; i < timetable->n; i++) {
    assert(nullptr, timetable->entries[i] != NULL);

    for (size_t j = 0; j < timetable->m; j++) {
      assert(nullptr, timetable->entries[i][j].classrooms != NULL);

      TimeBlock entry = timetable->entries[i][j];
      for (size_t k = 0; k < entry.s_classrooms; k++) {
        assert(nullptr, entry.classrooms[k].classes != NULL);

        ClassRoom classroom = entry.classrooms[k];
        for (size_t l = 0; l < classroom.s_classes; l++) {
          Class class = classroom.classes[l];

          if (class.students)
            for (size_t m = 0; m < class.s_students; m++)
              if (!included(class.students[m], *users, s_users)) {
                s_users = append(class.students[m], users, s_users);
                assert(nullptr, *users != NULL);
              }

          if (class.teachers)
            for (size_t m = 0; m < class.s_teachers; m++)
              if (!included(class.teachers[m], *users, s_users)) {
                s_users = append(class.teachers[m], users, s_users);
                assert(nullptr, *users != NULL);
              }

        }
      }
    }
  }

  return s_users;
}
