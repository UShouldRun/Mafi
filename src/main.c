#include "core.h"

int main(int argc, char *argv[]) {
  ExceptionType return_value = none;

  Logger logger;
  initLogger(&logger, stderr);
  
  Report *report = logger.createReport(status_type, info, none, nil);
  raise_stop(report == NULL);

  run_raise_exception(argc < 4, error, invalid_argument);

  FILE *logger_file = NULL;
  if (argc == 5 && argv[4] != NULL) {
    logger_file = fopen(argv[4], "w");
    run_log_custom_exception_msg(logger_file == NULL,  "could not open selected logger file '%s'", argv[4], error, io_error);
    logger.dest = logger_file;
  }

  size_t depth = 0;
  Rules rules;

  size_t s_users = 0;
  User **users = NULL;

  size_t s_possible_timetables = 0;
  TimeTable **possible_timetables = NULL, *root_state = NULL;

  size_t s_classrooms = 0, s_classes = 0;
  ID *classrooms = NULL, *classes = NULL;

  read(&logger, report, argv[2], &depth, &rules, &root_state, &classrooms, &classes, &users, &s_classrooms, &s_classes, &s_users);
  run_raise_exception(report->type == exception_type, report->result.exception.level, report->result.exception.type);
  logger_update_report_status((&logger), report, info, "Read input file successfully");

  // type_print_timetable(stdout, root_state);

  char *endptr = NULL;
  double acceptance_rate = strtod(argv[1], &endptr);
  run_raise_exception(*endptr != '\0', error, unexp_behaviour);

  mafi(&logger, report, acceptance_rate, &users, &possible_timetables, &s_users, &s_possible_timetables, depth, root_state, &rules);
  run_raise_exception(report->result.exception.level > warning, report->result.exception.level, report->result.exception.type);
  logger_update_report_status((&logger), report, info, "Performed search by Mafi successfully");

  store(&logger, report, argv[3], possible_timetables, s_possible_timetables);
  run_raise_exception(report->type == exception_type, report->result.exception.level, report->result.exception.type);
  logger_update_report_status((&logger), report, info, "Stored all data successfully");

_free:
  if (classrooms != NULL)
    free(classrooms);
  if (classes != NULL)
    free(classes);

  if (possible_timetables != NULL) {
    for (size_t i = 0; i < s_possible_timetables; i++)
      if (possible_timetables[i] != NULL)
        type_delete_timetable(&(possible_timetables[i]));
    free(possible_timetables);
  }
  
  if (users != NULL) {
    for (size_t i = 0; i < s_users; i++)
      if (users[i] != NULL)
        type_delete_user(&users[i]);
    free(users);
  }

  logger.writeReport(report, logger.dest);
  if (argc == 4 && logger_file != NULL)
    free(logger_file);

  if (report)
    logger.deleteLog(report, report_type);
  
_return:
  return (int)return_value;
}
