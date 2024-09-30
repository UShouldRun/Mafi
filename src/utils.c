#include "utils.h"

void check_snprintf(FILE *logger_file, int number_characters, size_t s_buffer) {
  if (!logger_file)
    logger_file = stderr;
  if (number_characters >= s_buffer)
    logger_panic(logger_file, "Buffer char complete_message too small", number_characters);
  else if (number_characters < 0)
    logger_panic(logger_file, "snprintf failed with encoding error or other issue", number_characters);
}

void _invalid_format(Logger *logger, Report *report, char *file_name, const char *file, const char *func, uint line) {
  if (!logger)
    logger_panic(stderr, "logger == NULL", 1);
  if (!file_name)
    return;

  char complete_message[256];
  size_t buffer_size = sizeof(complete_message),
         number_characters = snprintf(complete_message, sizeof(complete_message), "file '%s' has an invalid format", file_name);
  check_snprintf(logger->dest, number_characters, buffer_size); 

  if (!report) {
    Message *msg = logger->appendToMessageTrack(NULL, complete_message, exception_type);
    if (!msg)
      return;
    Exception exception = { error, invalid_argument, *msg };
    logger->writeException(&exception, stderr);
    logger->deleteMessage(msg);
  }

  logger->updateReport(report, exception_type, info, error, io_error, _logger_complete_message(complete_message, file, func, line));
}

bool valid_classes_in_all_classrooms(TimeTable *timetable, size_t max_classes) {
  for (size_t i = 0; i < timetable->n; i++) {
    TimeBlock *timeblocks = timetable->entries[i];
    if (!timeblocks)
      return false;

    for (size_t j = 0; j < timetable->m; j++) {
      ClassRoom *classrooms = timeblocks[j].classrooms;
      if (!classrooms)
        return false;

      for (size_t k = 0; k < timeblocks[j].s_classrooms; k++) {
        size_t s_classes = classrooms[k].s_classes;
        Class *classes = classrooms[k].classes;

        if (classes == NULL && s_classes != 0)
          return false;
        if (classes != NULL && s_classes == 0)
          return false;
        if (s_classes > max_classes)
          return false;
      }
    }
  }
  return true;
}

bool compare_timetable(void *stateA, void *stateB, void *context) {
  if (!stateA)
    return false;
  if (!stateB)
    return true;
  if (!context)
    return false;

  TimeTable *timetableA = (TimeTable *)stateA,
            *timetableB = (TimeTable *)stateB;
  Users *arr_users = (Users *)context;
  
  size_t sumA = 0, sumB = 0;
  for (size_t i = 0; i < arr_users->s_users; i++) {
    sumA += type_get_assign_from_timetable(NULL, timetableA, arr_users->users[i]);
    sumB += type_get_assign_from_timetable(NULL, timetableB, arr_users->users[i]);
  }

  return sumA < sumB;
}

bool less_than_max_students_per_classroom(size_t max_students_per_classroom, TimeBlock *timeblock, ID class) {
  if (!timeblock)
    return false;

  size_t s_classrooms = timeblock->s_classrooms;
  ClassRoom *classrooms = timeblock->classrooms;
  if (!classrooms)
    return false;

  for (size_t i = 0; i < s_classrooms; i++) {
    size_t s_classes = classrooms[i].s_classes;
    Class *classes = classrooms[i].classes;
    if (!classes)
      continue;

    for (size_t j = 0; j < s_classes; j++) {
      if (classes[j].id != class)
        continue;
      if (!classes[j].students)
        continue;
      if (classes[j].s_students + 1 > max_students_per_classroom)
        return false;
    }
  }
      
  return true;
}

bool less_than_max_teachers_per_classroom(size_t max_teachers_per_classroom, TimeBlock *timeblock, ID class) {
  if (!timeblock)
    return false;

  ClassRoom *classrooms = timeblock->classrooms;
  if (!classrooms)
    return false;

  for (size_t i = 0; i < timeblock->s_classrooms; i++) {
    if (!classrooms[i].classes)
      continue;

    Class *classes = classrooms[i].classes;
    for (size_t j = 0; j < classrooms[i].s_classes; j++) {
      if (classes[j].id != class)
        continue;
      if (!classes[j].teachers)
        continue;
      if (classes[j].s_teachers + 1 > max_teachers_per_classroom)
        return false;
    }
  }
      
  return true;
}

bool less_than_max_same_class_per_week_per_student(size_t max_same_class_per_week_per_student, TimeTable *timetable, ID class, ID id) {
  size_t count = 0;

  for (size_t i = 0; i < timetable->n; i++) {
    if (!timetable->entries[i])
      continue;

    for (size_t j = 0; j < timetable->m; j++) {
      TimeBlock timeblock = timetable->entries[i][j];
      ClassRoom *classrooms = timeblock.classrooms;

      if (!classrooms)
        continue;
 
      bool in_current_block = false;
      for (size_t k = 0; k < timeblock.s_classrooms; k++) {
        size_t s_classes = classrooms[k].s_classes;
        Class *classes = classrooms[k].classes;
        if (!classes)
          continue;

        for (size_t l = 0; l < s_classes; l++) {
          if (classes[l].id != class)
            continue;

          size_t s_students = classes[l].s_students;
          ID *students = classes[l].students;
          if (!students)
            continue;

          for (size_t m = 0; m < s_students; m++) {
            if (students[m] != id)
              continue;

            count++;
            in_current_block++;
            break;
          }
          
          break;
        }
      }

      if (in_current_block)
        break;
    }
  }

  return count + 1 <= max_same_class_per_week_per_student;
}

bool less_than_max_blocks_per_day_per_student(size_t max_blocks_per_day_per_student, TimeTable *timetable, size_t j, ID id) {
  if (!timetable)
    return false;

  size_t count = 0;
  for (size_t i = 0; i < timetable->n; i++) {
    if (!timetable->entries[i])
      continue;

    TimeBlock timeblock = timetable->entries[i][j];
    ClassRoom *classrooms = timeblock.classrooms;
    if (!classrooms)
      continue;

    bool in_current_block = false;
    for (size_t k = 0; k < timeblock.s_classrooms; k++) {
      size_t s_classes = classrooms[k].s_classes;
      Class *classes = classrooms[k].classes;
      if (!classes)
        continue;

      for (size_t l = 0; l < s_classes; l++) {
        size_t s_students = classes[l].s_students;
        ID *students = classes[l].students;
        if (!students)
          continue;

        for (size_t m = 0; m < s_students; m++) {
          if (students[m] != id)
            continue;

          count++;
          in_current_block++;
          break;
        }

        if (in_current_block)
          break;
      }

      if (in_current_block)
        break;
    }

    if (count + 1 > max_blocks_per_day_per_student)
      return false;
  }

  return true;
}

bool morning_and_afternoon(TimeTable *timetable, Entry *entry, ID id) {
  if (!timetable)
    return true;
  if (!entry)
    return true;

  size_t start = (entry->i > timetable->n/2) ? 0 : timetable->n/2, end = (entry->i > timetable->n/2) ? timetable->n/2 : timetable->n;
  for (size_t i = start; i < end; i++) {
    if (!timetable->entries[i])
      continue;

    TimeBlock timeblock = timetable->entries[i][entry->j];
    ClassRoom *classrooms = timeblock.classrooms;
    if (!classrooms)
      continue;

    for (size_t k = 0; k < timeblock.s_classrooms; k++) {
      size_t s_classes = classrooms[k].s_classes;
      Class *classes = classrooms[k].classes;
      if (!classes)
        continue;

      for (size_t l = 0; l < s_classes; l++) {
        size_t s_students = classes[l].s_students;
        ID *students = classes[l].students;
        if (!students)
          continue;

        for (size_t m = 0; m < s_students; m++)
          if (students[m] == id)
            return true;
      }
    }
  }

  return false;
}

bool already_in_class(TimeBlock *timeblock, ID id) {
  if (!timeblock)
    return true;

  size_t s_classrooms = timeblock->s_classrooms;
  ClassRoom *classrooms = timeblock->classrooms;
  if (!classrooms)
    return true;

  for (size_t k = 0; k < s_classrooms; k++) {
    size_t s_classes = classrooms[k].s_classes;
    Class *classes = classrooms[k].classes;
    if (!classes)
      continue;

    for (size_t l = 0; l < s_classes; l++) {
      size_t s_students = classes[l].s_students,
             s_teachers = classes[l].s_teachers;
      ID *students = classes[l].students,
         *teachers = classes[l].teachers;

      if (students)
        for (size_t m = 0; m < s_students; m++)
          if (students[m] == id)
            return true;

      if (teachers)
        for (size_t m = 0; m < s_teachers; m++)
          if (teachers[m] == id)
            return true;
    }
  }

  return false;
}

bool in_rules(void *rules, TimeTable *timetable, Entry *entry, ID class, ID user_id, bool is_student) {
  if (!rules)
    return false;
  if (!timetable)
    return false;
  if (!entry)
    return false;

  Rules *_rules = (Rules *)rules;
  TimeBlock timeblock = timetable->entries[entry->i][entry->j];

  bool rule1 = less_than_max_students_per_classroom(_rules->max_students_per_classroom, &timeblock, class),
       rule2 = less_than_max_same_class_per_week_per_student(_rules->max_same_class_per_week_per_student, timetable, class, user_id),
       rule3 = less_than_max_blocks_per_day_per_student(_rules->max_blocks_per_day_per_student, timetable, entry->j, user_id),
       rule4 = !morning_and_afternoon(timetable, entry, user_id),
       rule5 = less_than_max_teachers_per_classroom(_rules->max_teachers_per_classroom, &timeblock, class),
       rule6 = !already_in_class(&timeblock, user_id);

  return (is_student ? rule1 * rule2 * rule3 * (_rules->morning_and_afternoon ? true : rule4) : rule5) * rule6;
}

size_t get_classrooms(ID **available_classrooms, TimeTable *timetable, Entry *block, ID class, size_t max_size) {
  if (!timetable) {
    *available_classrooms = NULL;
    return 0;
  }
  if (!block) {
    *available_classrooms = NULL;
    return 0;
  }

  bool valid_entry = (bool)((block->i >= 0 && block->i < timetable->n) && (block->j >= 0 && block->j < timetable->m));
  if (!valid_entry) {
    *available_classrooms = NULL;
    return 0;
  }
  
  const size_t s_classrooms = timetable->entries[block->i][block->j].s_classrooms;
  ClassRoom *classrooms = timetable->entries[block->i][block->j].classrooms;
  if (!classrooms) {
    *available_classrooms = NULL;
    return 0;
  }

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

    bool has_class = false;
    for (size_t j = 0; j < s_classes; j++)
      if (classes[j].id == class) {
        has_class++;
        break;
      }

    if (!has_class)
      continue;

    if (s_avail_classrooms >= capacity) {
      ID *new_temp = (ID *)realloc(temp, (2 * capacity) * sizeof(ID));
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
    ID *new_temp = (ID *)realloc(temp, s_avail_classrooms * sizeof(ID));
    if (!new_temp) {
      *available_classrooms = NULL;
      return 0;
    }
    temp = new_temp;
  }

  *available_classrooms = temp;
  return s_avail_classrooms;
}
