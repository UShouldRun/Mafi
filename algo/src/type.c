#include "type.h"

Class *type_create_class(ID id, ID *students, ID *teachers, size_t s_students, size_t s_teachers) {
  Class *class = (Class *)malloc(sizeof(Class));
  if (!class)
    return NULL;
  class->id = id;
  class->students = students;
  class->teachers = teachers;
  class->len_students = s_students;
  class->len_teachers = s_teachers;
  return class;
}

ClassRoom *type_create_classroom(ID id, Class *classes, size_t s_classes) {
  ClassRoom *classroom = (ClassRoom *)malloc(sizeof(ClassRoom));
  if (!classroom)
    return NULL;
  classroom->id = id;
  classroom->classes = classes;
  classroom->len = s_classes;
  return classroom;
}

TimeBlock *type_create_timeblock(Entry *entry, ClassRoom *classrooms, size_t s_classrooms) {
  TimeBlock *timeblock = (TimeBlock *)malloc(sizeof(TimeBlock));
  if (!timeblock)
    return NULL;
  if (entry)
    timeblock->entry = *entry;
  else {
    timeblock->entry.i = 0;
    timeblock->entry.j = 0;
  }
  timeblock->classrooms = classrooms;
  timeblock->len = s_classrooms;
  return timeblock;
}

TimeTable *type_create_timetable(TimeBlock **entries, size_t n, size_t m) {
  TimeTable *timetable = (TimeTable *)malloc(sizeof(TimeTable));
  if (!timetable)
    return NULL;
  timetable->entries = entries;
  timetable->n = n;
  timetable->m = m;
  return timetable;
}

Student *type_create_student(ID id, ID *classes, Entry *avail, Assignment *assign, size_t s_classes, size_t s_avail, size_t s_assign) {
  Student *student = (Student *)malloc(sizeof(Student));
  if (!student)
    return NULL;
  student->id = id;
  student->classes = classes;
  student->avail = avail;
  student->assign = assign;
  student->len_classes = s_classes;
  student->len_avail = s_avail;
  student->len_assign = s_assign;
  return student;
}

Teacher *type_create_teacher(ID id, ID *classes, Entry *avail, Assignment *assign, size_t s_classes, size_t s_avail, size_t s_assign) {
  Teacher *teacher = (Teacher *)malloc(sizeof(Teacher));
  if (!teacher)
    return NULL;
  teacher->id = id;
  teacher->classes = classes;
  teacher->avail = avail;
  teacher->assign = assign;
  teacher->len_classes = s_classes;
  teacher->len_avail = s_avail;
  teacher->len_assign = s_assign;
  return teacher;
}

User *type_create_user(
  UserType type,
  ID id, ID *classes, Entry *avail, Assignment *assign,
  size_t s_classes, size_t s_avail, size_t s_assign
) {
  Student *student;
  Teacher *teacher;

  User *user = (User *)malloc(sizeof(User));
  if (!user)
    return NULL;
  user->type = type;

  switch (type) {
    case student_type:
      student = type_create_student(id, classes, avail, assign, s_classes, s_avail, s_assign);
      if (student == NULL)
        return NULL;
      user->select.student = *student;
      free(student);
      break;

    case teacher_type:
      teacher = type_create_teacher(id, classes, avail, assign, s_classes, s_avail, s_assign);
      if (teacher == NULL)
        return NULL;
      user->select.teacher = *teacher;
      free(teacher);
      break;

    default:
      return NULL;
  }

  return user;
}

ID *type_copy_user_ids(ID *users, size_t s_users) {
  if (!users)
    return NULL;

  ID *copy = (ID *)malloc(s_users*sizeof(ID));
  if (!copy)
    return NULL;

  for (size_t i = 0; i < s_users; i++)
    copy[i] = users[i];

  return copy;
}

Class *type_copy_classes(Class *classes, size_t s_classes) {
  if (!classes)
    return NULL;

  Class *copy = (Class *)malloc(s_classes*sizeof(Class));
  if (!copy)
    return NULL;

  for (size_t i = 0; i < s_classes; i++) {
    copy[i].id = classes[i].id;
    copy[i].len_students = classes[i].len_students;
    copy[i].len_teachers = classes[i].len_teachers;

    copy[i].students = type_copy_user_ids(classes[i].students, classes[i].len_students);
    copy[i].teachers = type_copy_user_ids(classes[i].teachers, classes[i].len_teachers);
  }

  return copy;
}

ClassRoom *type_copy_classrooms(ClassRoom *classrooms, size_t s_classrooms) {
  if (!classrooms)
    return NULL;

  ClassRoom *copy = (ClassRoom *)malloc(s_classrooms*sizeof(ClassRoom));
  if (!copy)
    return NULL;

  for (size_t i = 0; i < s_classrooms; i++) {
    copy[i].id = classrooms[i].id;
    copy[i].len = classrooms[i].len;
    copy[i].classes = type_copy_classes(classrooms[i].classes, classrooms[i].len);
  }

  return copy;
}

TimeBlock *type_copy_timeblocks(TimeBlock *timeblocks, size_t s_timeblocks) {
  if (!timeblocks)
    return NULL;

  TimeBlock *copy = (TimeBlock *)malloc(s_timeblocks*sizeof(TimeBlock));
  if (!copy)
    return NULL;

  for (size_t i = 0; i < s_timeblocks; i++) {
    copy[i].entry.i = timeblocks[i].entry.i;
    copy[i].entry.j = timeblocks[i].entry.j;
    copy[i].len = timeblocks[i].len;
    copy[i].classrooms = type_copy_classrooms(timeblocks[i].classrooms, timeblocks[i].len);
  }

  return copy;
}

TimeTable *type_copy_timetable(TimeTable *timetable) {
  if (!timetable)
    return NULL;

  TimeTable *copy = (TimeTable *)malloc(sizeof(TimeTable));
  if (!copy)
    return NULL;

  copy->n = timetable->n;
  copy->m = timetable->m;

  copy->entries = (TimeBlock **)malloc(copy->n*sizeof(TimeBlock *));
  if (!copy->entries) {
    free(copy);
    return NULL;
  }

  for (size_t i = 0; i < copy->n; i++) {
    copy->entries[i] = type_copy_timeblocks(timetable->entries[i], timetable->m);
    if (!copy->entries[i]) {
      for (size_t j = 0; j < i; j++)
        type_delete_timeblock(&copy->entries[j], i);
      free(copy->entries);
      free(copy);
      return NULL;
    }
  }

  return copy;
}

bool type_get_is_student(User *user) {
  return user->type == student_type;
}

size_t type_get_len_classes(User *user) {
  return user ? (user->type ? user->select.student.len_classes : user->select.teacher.len_classes) : 0;
}

size_t type_get_len_avail(User *user) {
  return user ? (user->type ? user->select.student.len_avail : user->select.teacher.len_avail) : 0;
}

size_t type_get_len_assign(User *user) {
  return user ? (user->type ? user->select.student.len_assign : user->select.teacher.len_assign) : 0;
}

size_t type_get_classroom_current_capacity(ClassRoom *classroom) {
  if (!classroom)
    return 0;

  size_t classroom_capacity = 0;
  for (size_t i = 0; i < classroom->len; i++) {
    if (!classroom->classes[i].students)
      continue;
    classroom_capacity += classroom->classes[i].len_students;
  }

  return classroom_capacity;
}

ID type_get_id(User *user) {
  return user ? (user->type ? user->select.student.id : user->select.teacher.id) : 0;
}

ID *type_get_classes(User *user) {
  return user ? (user->type ? user->select.student.classes : user->select.teacher.classes) : NULL;
}

Entry *type_get_avail(User *user) {
  return user ? (user->type ? user->select.student.avail : user->select.teacher.avail) : NULL;
}

Assignment *type_get_assign(User *user) {
  return user ? (user->type ? user->select.student.assign : user->select.teacher.assign) : NULL;
}

size_t type_get_assign_from_timetable(Assignment **assign, TimeTable *timetable, User *user) {
  if (!timetable)
    return 0;
  if (!user)
    return 0;
  if (user->type != student_type && user->type != teacher_type)
    return 0;

  size_t size = 0;
  for (size_t i = 0; i < timetable->n; i++) {
    if (!timetable->entries[i])
      return 0;

    for (size_t j = 0; j < timetable->m; j++) {
      if (!timetable->entries[i][j].classrooms)
        return 0;

      for (size_t k = 0; k < timetable->entries[i][j].len; k++) {
        if (!timetable->entries[i][j].classrooms[k].classes)
          return 0;
        
        for (size_t l = 0; l < timetable->entries[i][j].classrooms[k].len; l++) {
          if (!timetable->entries[i][j].classrooms[k].classes[l].students)
            return 0;
          if (!timetable->entries[i][j].classrooms[k].classes[l].teachers)
            return 0;
          
          switch (user->type) {
            case student_type:
              for (size_t m = 0; m < timetable->entries[i][j].classrooms[k].classes[l].len_students; m++) {
                if (timetable->entries[i][j].classrooms[k].classes[l].students[m] == type_get_id(user)) {
                  size++;
                  if (assign) {
                    Assignment *temp = (Assignment *)realloc(*assign, size*sizeof(Assignment));
                    if (!temp)
                      return 0;
                    *assign = temp;
                    (*assign)[size-1].block.i = i;
                    (*assign)[size-1].block.j = j;
                    (*assign)[size-1].classroom = timetable->entries[i][j].classrooms[k].id;
                    (*assign)[size-1].class = timetable->entries[i][j].classrooms[k].classes[l].id;
                  }
                }
              }
              break;

            case teacher_type:
              for (size_t m = 0; m < timetable->entries[i][j].classrooms[k].classes[l].len_teachers; m++) {
                if (timetable->entries[i][j].classrooms[k].classes[l].teachers[m] == type_get_id(user)) {
                  size++;
                  if (assign) {
                    Assignment *temp = (Assignment *)realloc(*assign, size*sizeof(Assignment));
                    if (!temp)
                      return 0;
                    *assign = temp;
                    (*assign)[size-1].block.i = i;
                    (*assign)[size-1].block.j = j;
                    (*assign)[size-1].classroom = timetable->entries[i][j].classrooms[k].id;
                    (*assign)[size-1].class = timetable->entries[i][j].classrooms[k].classes[l].id;
                  }
                }
              }
              break;

            default:
              break;
          }
        }
      }
    } 
  }
    
  return size;
}

size_t type_get_assign_from_timetable_with_id(Assignment **assign, TimeTable *timetable, ID user) {
  if (!assign)
    return 0;
  if (!timetable)
    return 0;

  size_t s_schedule = 0;
  Assignment *schedule = NULL; 
  for (size_t i = 0; i < timetable->n; i++) {
    if (!timetable->entries[i])
      return 0;

    for (size_t j = 0; j < timetable->m; j++) {
      if (!timetable->entries[i][j].classrooms)
        continue;

      bool checked_user = false;
      for (size_t k = 0; k < timetable->entries[i][j].len; k++) {
        ClassRoom classroom = timetable->entries[i][j].classrooms[k];

        if (!classroom.classes)
          continue;

        for (size_t l = 0; l < classroom.len; l++) {
          Class class = classroom.classes[l]; 
          if (class.students) {
            for (size_t m = 0; m < class.len_students; m++)
              if (class.students[m] == user) {
                Assignment *temp = (Assignment *)realloc(schedule, (s_schedule + 1) * sizeof(Assignment));
                if (!temp)
                  return 0;
                schedule = temp;
                schedule[s_schedule].block = timetable->entries[i][j].entry;
                schedule[s_schedule].classroom = classroom.id;
                schedule[s_schedule++].class = class.id;
                checked_user++;
                break;
              }
          }
          if (class.teachers) {
            for (size_t m = 0; m < class.len_teachers; m++)
              if (class.teachers[m] == user) {
                Assignment *temp = (Assignment *)realloc(schedule, (s_schedule + 1) * sizeof(Assignment));
                if (!temp)
                  return 0;
                schedule = temp;
                schedule[s_schedule].block = timetable->entries[i][j].entry;
                schedule[s_schedule].classroom = classroom.id;
                schedule[s_schedule++].class = class.id;
                checked_user++;
                break;
              }
          }
          if (checked_user)
            break;
        }

        if (checked_user)
          break;
      }
    }
  }

  *assign = schedule;
  return s_schedule;
}

void type_set_len_classes(User *user, size_t len) {
  if (!user) return;
  switch (user->type) {
    case student_type:
      user->select.student.len_classes = len;
      return;
    case teacher_type:
      user->select.teacher.len_classes = len;
      return;
    default:
      return;
  }
}

void type_set_len_avail(User *user, size_t len) {
  if (!user) return;
  switch (user->type) {
    case student_type:
      user->select.student.len_avail = len;
      return;
    case teacher_type:
      user->select.teacher.len_avail = len;
      return;
    default:
      return;
  }
}

void type_set_len_assign(User *user, size_t len) {
  if (!user) return;
  switch (user->type) {
    case student_type:
      user->select.student.len_assign = len;
      return;
    case teacher_type:
      user->select.teacher.len_assign = len;
      return;
    default:
      return;
  }
}

void type_set_id(User *user, ID id) {
  if (!user) return;
  switch (user->type) {
    case student_type:
      user->select.student.id = id;
      return;
    case teacher_type:
      user->select.teacher.id = id;
      return;
    default:
      return;
  }
}

void type_set_classes(User *user, ID *classes) {
  if (!user) return;
  switch (user->type) {
    case student_type:
      user->select.student.classes = classes;
      return;
    case teacher_type:
      user->select.teacher.classes = classes;
      return;
    default:
      return;
  }
}

void type_set_avail(User *user, Entry *avail) {
  if (!user) return;
  switch (user->type) {
    case student_type:
      user->select.student.avail = avail;
      return;
    case teacher_type:
      user->select.teacher.avail = avail;
      return;
    default:
      return;
  }
}

void type_set_assign(User *user, Assignment *assign) {
  if (!user) return;
  switch (user->type) {
    case student_type:
      user->select.student.assign = assign;
      return;
    case teacher_type:
      user->select.teacher.assign = assign;
      return;
    default:
      return;
  }
}

void type_delete_class(Class **class, size_t len) {
  if (!class)
    return;
  if (!*class)
    return;

  for (size_t i = 0; i < len; i++) {
    if ((*class)[i].students)
      free((*class)[i].students);
    if ((*class)[i].teachers)
      free((*class)[i].teachers);
  }
  free(*class);

  *class = NULL;
}

void type_delete_classroom(ClassRoom **classroom, size_t len) {
  if (!classroom)
    return;
  if (!*classroom)
    return;

  for (size_t i = 0; i < len; i++)
    type_delete_class(&(*classroom)[i].classes, (*classroom)[i].len);
  free(*classroom);

  *classroom = NULL;
}

void type_delete_timeblock(TimeBlock **timeblock, size_t len) {
  if (!timeblock)
    return;
  if (!*timeblock)
    return;

  for (size_t j = 0; j < len; j++)
    type_delete_classroom(&(*timeblock)[j].classrooms, (*timeblock)[j].len);
  free(*timeblock);

  *timeblock = NULL;
}

void type_delete_timetable(TimeTable **timetable) {
  if (!timetable)
    return;
  if (!*timetable)
    return;

  for (size_t i = 0; i < (*timetable)->n; i++)
    type_delete_timeblock(&(*timetable)->entries[i], (*timetable)->m);
  free((*timetable)->entries);
  free(*timetable);

  *timetable = NULL;
}

void type_delete_student(Student **student) {
  if (!student)
    return;
  if (!*student)
    return;
  if (!(*student)->classes)
    free((*student)->classes);
  if (!(*student)->avail)
    free((*student)->avail);
  if (!(*student)->assign)
    free((*student)->assign);
  free(*student);
  *student = NULL;
}

void type_delete_teacher(Teacher **teacher) {
  if (!teacher)
    return;
  if (!*teacher)
    return;
  if (!(*teacher)->classes)
    free((*teacher)->classes);
  if (!(*teacher)->avail)
    free((*teacher)->avail);
  if (!(*teacher)->assign)
    free((*teacher)->assign);
  free(*teacher);
  *teacher = NULL;
}

void type_delete_user(User **user) {
  if (!user)
    return;
  if (!*user)
    return;
  switch ((*user)->type) {
    case student_type: {
      if ((*user)->select.student.classes)
        free((*user)->select.student.classes);
      if ((*user)->select.student.avail)
        free((*user)->select.student.avail);
      if ((*user)->select.student.assign)
        free((*user)->select.student.assign);
      break;
    } 
    case teacher_type: {
      if ((*user)->select.teacher.classes)
        free((*user)->select.teacher.classes);
      if ((*user)->select.teacher.avail)
        free((*user)->select.teacher.avail);
      if ((*user)->select.teacher.assign)
        free((*user)->select.teacher.assign);
      break;
    }
    default:
      return;
  }
  free(*user);
  *user = NULL;
}

void type_print_timetable(FILE *dest, TimeTable *timetable) {
  if (!dest) {
    fprintf(stderr, "[Error]: cannot print timetable to dest file (dest == NULL)\n");
    return;
  }
  if (!timetable) {
    fprintf(stderr, "[Error]: cannot print timetable to dest file (timetable == NULL)\n");
    return;
  }

  fprintf(dest, "n:%zu, m:%zu\n", timetable->n, timetable->m);
  for (size_t i = 0; i < timetable->n; i++) {
    TimeBlock *timeblocks = timetable->entries[i];
    fprintf(dest, "\t\033[37mi:%zu, timeblocks:%p\033[0m\n", i, (void *)timetable->entries[i]);
    if (!timeblocks)
      continue;

    for (size_t j = 0; j < timetable->m; j++) {
      size_t s_classrooms = timeblocks[j].len;
      ClassRoom *classrooms = timeblocks[j].classrooms;
      fprintf(dest, "\t\t\033[35mj:%zu, classrooms:%p (s_classrooms:%zu)\033[0m\n", j, (void *)classrooms, s_classrooms);
      if (!classrooms)
        continue;

      for (size_t k = 0; k < s_classrooms; k++) {
        ID classroom_id = classrooms[k].id;
        size_t s_classes = classrooms[k].len;
        Class *classes = classrooms[k].classes;
        fprintf(dest, "\t\t\t\033[36mclassroom_id:%u, classes:%p (s_classes:%zu)\033[0m\n", classroom_id, (void *)classes, s_classes);
        if (!classes)
          continue;

        for (size_t l = 0; l < s_classes; l++) {
          ID class_id = classes[l].id;

          size_t s_students = classes[l].len_students;
          size_t s_teachers = classes[l].len_teachers;

          ID *students = classes[l].students,
             *teachers = classes[l].teachers;

          fprintf(
            dest,
            "\t\t\t\t\033[33mclass_id:%u, students:%p (s_students:%zu), teachers:%p (s_teachers:%zu)\033[0m\n",
            class_id, (void *)students, s_students, (void *)teachers, s_teachers
          );

          if (students)
            for (size_t m = 0; m < s_students; m++)
              fprintf(dest, "\t\t\t\t\t\033[032mstudent_id:%u\033[0m\n", students[m]);

          if (teachers)
            for (size_t m = 0; m < s_teachers; m++)
              fprintf(dest, "\t\t\t\t\t\033[31mteacher_id:%u\033[0m\n", teachers[m]);
        }
      }
    }
  }
}
