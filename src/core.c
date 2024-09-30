#include "core.h"

void read(
  Logger *logger, Report *report,
  char *read_file_name, size_t *depth, Rules *rules, TimeTable **root_state,
  ID **classrooms, ID **classes, User ***users,
  size_t *s_classrooms, size_t *s_classes, size_t *s_users
) {
  FILE *read_file = NULL;

  raise_ptr_exception(read_file_name == NULL);
  raise_ptr_exception(depth == NULL);
  raise_ptr_exception(rules == NULL);
  raise_ptr_exception(root_state == NULL);
  raise_ptr_exception(classrooms == NULL);
  raise_ptr_exception(classes == NULL);
  raise_ptr_exception(users == NULL);
  raise_ptr_exception(s_classrooms == NULL);
  raise_ptr_exception(s_classes == NULL);
  raise_ptr_exception(s_users == NULL)

  log_custom_status_message("opening selected read file '%s'", read_file_name);
  
  read_file = fopen(read_file_name, "r");
  log_custom_exception_msg(read_file == NULL, "could not open selected read file '%s'", read_file_name, error, io_error);
  log_custom_status_message("successfully opened selected read file '%s'", read_file_name);

  *s_classes = 0;
  *s_users = 0;

  logger_update_report_status(logger, report, info, "reading file");

  if (fscanf(read_file, "%zu\n\n", depth) == EOF)
    invalid_format(read_file_name);

  if (fscanf(
      read_file, "%zu %zu %zu %zu %zu %u\n\n",
      &rules->max_students_per_classroom,
      &rules->max_teachers_per_classroom,
      &rules->max_same_class_per_week_per_student,
      &rules->max_classes_per_classroom,
      &rules->max_blocks_per_day_per_student,
      &rules->morning_and_afternoon
    ) == EOF)
  {
    invalid_format(read_file_name);
  }

  size_t n = 0, m = 0;
  if (fscanf(read_file, "%zu %zu\n", &m, &n) == EOF)
    invalid_format(read_file_name);

  if (fscanf(read_file, "%zu\n", s_classrooms) == EOF)
    invalid_format(read_file_name);

  *classrooms = (ID *)malloc((*s_classrooms) * sizeof(ID));
  raise_exception(*classrooms == NULL, fatal, mem_error);

  for (size_t i = 0; i < *s_classrooms; i++)
    if (fscanf(read_file, "\t%u\n", &((*classrooms)[i])) == EOF)
      invalid_format(read_file_name);

  TimeBlock **entries = (TimeBlock **)malloc(n * sizeof(TimeBlock *));
  raise_exception(entries == NULL, fatal, mem_error);

  for (size_t i = 0; i < n; i++) {
    entries[i] = (TimeBlock *)malloc(m * sizeof(TimeBlock));
    raise_exception(entries[i] == NULL, fatal, mem_error);

    for (size_t j = 0; j < m; j++) {
      entries[i][j].entry.i = i;
      entries[i][j].entry.j = j;
      entries[i][j].s_classrooms = *s_classrooms;

      entries[i][j].classrooms = (ClassRoom *)malloc((*s_classrooms) * sizeof(ClassRoom));
      raise_exception(entries[i][j].classrooms == NULL, fatal, mem_error);

      for (size_t k = 0; k < *s_classrooms; k++) {
        entries[i][j].classrooms[k].id = (*classrooms)[k];
        entries[i][j].classrooms[k].s_classes = 0;
        entries[i][j].classrooms[k].classes = NULL;
      }
    }
  }

  *root_state = type_create_timetable(entries, n, m);
  raise_exception(*root_state == NULL, fatal, mem_error);

  if (fscanf(read_file, "\n%zu\n", s_classes) == EOF)
    invalid_format(read_file_name);

  *classes = (ID *)malloc((*s_classes) * sizeof(ID));
  raise_exception(classes == NULL, fatal, mem_error);

  for (size_t i = 0; i < *s_classes; i++) {
    ID class_i = 0;
    if (fscanf(read_file, "\t%u\n", &class_i) == EOF)
      invalid_format(read_file_name);

    (*classes)[i] = class_i;

    size_t s_locations = 0;
    if (fscanf(read_file, "\t\t%zu\n", &s_locations) == EOF)
      invalid_format(read_file_name);

    for (size_t j = 0; j < s_locations; j++) {
      Entry entry;
      ID classroom;
      if (fscanf(read_file, "\t\t\t%zu %zu %u\n", &entry.i, &entry.j, &classroom) == EOF)
        invalid_format(read_file_name);

      raise_exception(entry.i >= n, error, invalid_argument);
      raise_exception(entry.j >= m, error, invalid_argument);

      TimeBlock timeblock = (*root_state)->entries[entry.i][entry.j];
      ClassRoom *tb_classrooms = timeblock.classrooms;
      for (size_t k = 0; k < timeblock.s_classrooms ; k++) {
        if (tb_classrooms[k].id != classroom)
          continue;

        size_t s_classes = tb_classrooms[k].s_classes;
        Class *tb_classes = (Class *)realloc(tb_classrooms[k].classes, (s_classes + 1) * sizeof(Class));
        raise_exception(tb_classes == NULL, fatal, mem_error);

        tb_classes[s_classes].id = class_i;
        tb_classes[s_classes].s_students = 0;
        tb_classes[s_classes].s_teachers = 0;
        tb_classes[s_classes].students = NULL;
        tb_classes[s_classes].teachers = NULL;

        tb_classrooms[k].s_classes++;
        tb_classrooms[k].classes = tb_classes;
      }
    }
  }

  if (!valid_classes_in_all_classrooms(*root_state, rules->max_classes_per_classroom)) {
    logger_update_report_exception(logger, report, error, unexp_behaviour, "root timetable has invalid classes");
    goto _return;
  }

  if (fscanf(read_file, "\n%zu\n", s_users) == EOF)
    invalid_format(read_file_name);

  *users = (User **)realloc(*users, (*s_users) * sizeof(User *));
  raise_exception(*users == NULL, fatal, mem_error);

  for (size_t i = 0; i < *s_users; i++) {
    uint user_type = 0;
    ID current_id = 0,
       current_class = 0,
       *user_classes = NULL;
    Entry *current_avail = NULL;
    size_t s_current_classes = 0,
           s_current_avail = 0,
           entry1 = 0,
           entry2 = 0;

    if (fscanf(read_file, "\t%u %u\n", &user_type, &current_id) == EOF)
      invalid_format(read_file_name);

    if (fscanf(read_file, "\t\t%zu\n", &s_current_classes) == EOF)
      invalid_format(read_file_name);

    user_classes = (ID *)malloc(s_current_classes * sizeof(ID));
    raise_exception(user_classes == NULL, fatal, mem_error);
    for (size_t i = 0; i < s_current_classes; i++) {
      if (fscanf(read_file, "\t\t\t%u\n", &current_class) == EOF)
        invalid_format(read_file_name);
      user_classes[i] = current_class;
    }

    if (fscanf(read_file, "\t\t%zu\n", &s_current_avail) == EOF)
      invalid_format(read_file_name);

    current_avail = (Entry *)malloc(s_current_avail * sizeof(Entry));
    raise_exception(current_avail == NULL, fatal, mem_error);
    for (size_t i = 0; i < s_current_avail; i++) {
      if (fscanf(read_file, "\t\t\t%zu %zu\n", &entry1, &entry2) == EOF)
        invalid_format(read_file_name);
      current_avail[i].i = entry1;
      current_avail[i].j = entry2;
    }

    User *user = type_create_user(user_type, current_id, user_classes, current_avail, s_current_classes, s_current_avail);
    raise_exception(user == NULL, fatal, mem_error)

    (*users)[i] = user;
  }

  log_custom_status_message("successfully scanned selected read file (total scanned users: %zu)", *s_users);

_return:
  if (read_file)
    fclose(read_file);
}

void store(Logger *logger, Report *report, char *write_file_name, TimeTable **timetables, size_t s_timetables) {
  FILE *write_file = NULL;

  raise_ptr_exception(write_file_name == NULL);
  raise_ptr_exception(timetables == NULL);

  log_custom_status_message("opening selected write file '%s'", write_file_name);

  write_file = fopen(write_file_name, "w");
  log_custom_exception_msg(write_file == NULL, "could not open selected read file '%s'", write_file_name, error, io_error);
  log_custom_status_message("successfully opened selected write file '%s'", write_file_name);

  fprintf(write_file, "%zu\n", s_timetables);
  for (size_t number = 0; number < s_timetables; number++) {
    TimeTable *timetable = timetables[number];
    if (timetable == NULL)
      continue;
    fprintf(write_file, "%zu %zu\n", timetable->n, timetable->m);

    for (size_t i = 0; i < timetable->n; i++) {
      raise_ptr_exception(timetable->entries[i] == NULL);

      for (size_t j = 0; j < timetable->m; j++) {
        fprintf(write_file, "\t%zu %zu %zu\n", i, j, timetable->entries[i][j].s_classrooms);

        ClassRoom *classrooms = timetable->entries[i][j].classrooms;
        raise_ptr_exception(classrooms == NULL);

        for (size_t k = 0; k < timetable->entries[i][j].s_classrooms; k++) {
          fprintf(write_file, "\t\t%u %zu\n", classrooms[k].id, classrooms[k].s_classes);

          Class *classes = classrooms[k].classes;
          for (size_t l = 0; l < classrooms[k].s_classes; l++) {
            fprintf(write_file, "\t\t\t%u %zu %zu\n", classes[l].id, classes[l].s_students, classes[l].s_teachers);

            size_t s_students = classes[l].s_students, s_teachers = classes[l].s_teachers;
            ID *students = classes[l].students,
               *teachers = classes[l].teachers;

            if (students != NULL)
              for (size_t m = 0; m < s_students; m++)
                fprintf(write_file, "\t\t\t\t%u\n", students[m]);

            fprintf(write_file, "\n");

            if (teachers != NULL)
              for (size_t m = 0; m < s_teachers; m++)
                fprintf(write_file, "\t\t\t\t%u\n", teachers[m]);

            fprintf(write_file, "\n");
          }
          fprintf(write_file, "\n");
        }
        fprintf(write_file, "\n");
      }
      fprintf(write_file, "\n");
    }
  }

_return:
  if (write_file)
    fclose(write_file);
}

void mafi(
  Logger *logger, Report *report, double acceptance_rate,
  User ***users, TimeTable ***possible_timetables,
  size_t *s_users, size_t *s_possible_timetables,
  size_t depth, TimeTable *root_state, Rules *rules
) {
  raise_ptr_exception(users == NULL);
  raise_ptr_exception(*users == NULL);
  raise_ptr_exception(possible_timetables == NULL);
  raise_ptr_exception(s_users == NULL);
  raise_ptr_exception(s_possible_timetables == NULL);

  Exception *exception = logger->createException(none, nil);

  logger_update_report_status(logger, report, info, "Starting Mafi");
  
  size_t s_schedules = 0;
  Assignment *schedules = NULL;

  User *user = NULL;
  Users context_users = {
    .users = *users,
    .s_users = *s_users
  };

  Tree *search_tree = tree_library_create_root((void *)root_state), *leaf = NULL;

  for (size_t i = 0; i < depth; i++) {
    log_custom_status_message("Mafi iteration %zu", i);
        
    algo_queue(logger, exception, users, s_users);
    handle_exception(exception);
    
    logger_update_report_status(logger, report, info, "queued users");

    algo_pick(logger, exception, &user, *users, *s_users);
    handle_exception(exception);

    logger_update_report_status(logger, report, info, "picked top queue user");

    context_users.users = *users;
    context_users.s_users = *s_users;
    algo_a_star_best(logger, exception, search_tree, &leaf, &context_users, compare_timetable);
    handle_exception(exception);

    logger_update_report_status(logger, report, info, "picked best leaf node");

    algo_search(
      logger, exception,
      user, (TimeTable *)leaf->state,
      &schedules, &s_schedules,
      rules->max_students_per_classroom, (void *)rules,
      in_rules, get_classrooms
    );
    handle_exception(exception);

    logger_update_report_status(logger, report, info, "searched leaf node");

    for (size_t i = 0; i < s_schedules; i++) {
      algo_update_tree(logger, exception, &leaf, user, &schedules[i]);
      handle_exception(exception);

      algo_update_timetables(logger, exception, search_tree, possible_timetables, *users, s_possible_timetables, *s_users);
      handle_exception(exception);
    }

    // maybe this needs a change
    type_set_size_assign(user, type_get_size_assign(user) + 1);

    logger_update_report_status(logger, report, info, "updated tree and timetables");

    free(schedules);
  }

  *users = context_users.users;
  *s_users = context_users.s_users;

  if (*possible_timetables == NULL) {
    logger_update_report_status(logger, report, info, "No timetable solutions were found");

    size_t s_leaves = 0;
    Tree **leaves = tree_library_get_leaves(search_tree, &s_leaves);

    TimeTable **temp = (TimeTable **)malloc(s_leaves * sizeof(TimeTable *));
    raise_ptr_exception(temp == NULL);

    size_t s_total_classes = 0;
    for (size_t i = 0; i < *s_users; i++)
      s_total_classes += type_get_is_student((*users)[i]) ? type_get_size_classes((*users)[i]) : 1;

    *s_possible_timetables = 0;
    for (size_t i = 0; i < s_leaves; i++) {
      TimeTable *state = (TimeTable *)leaves[i]->state;

      size_t s_total_assigns = 0;
      for (size_t j = 0; j < *s_users; j++)
        s_total_assigns += type_get_assign_from_timetable(NULL, state, (*users)[j]);

      if (s_total_assigns < s_total_classes * acceptance_rate)
        continue;
      
      TimeTable *copy = type_copy_timetable(state);
      raise_ptr_exception(copy == NULL);
      temp[(*s_possible_timetables)++] = copy;
    }
    if (*s_possible_timetables == 0) {
      free(temp);
      temp = NULL;
      raise_exception(*s_possible_timetables == 0, error, unexp_behaviour);
    } else if (*s_possible_timetables != s_leaves) {
      TimeTable **new_temp = (TimeTable **)realloc(temp, (*s_possible_timetables) * sizeof(TimeTable *));
      raise_exception(new_temp == NULL, warning, null_pointer);
      temp = new_temp;
    }

    *possible_timetables = temp;
    free(leaves);
  }

_return:
  if (search_tree)
    tree_library_delete_tree(&search_tree, algo_free_state);
  if (exception)
    logger->deleteException(exception);
}
