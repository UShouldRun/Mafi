#ifndef UTILS_H
#define UTILS_H

#include "algo.h"

#define invalid_format(file_name) \
      do { \
        _invalid_format(logger, report, file_name, __FILE__, __func__, __LINE__); \
        goto _return; \
      } while (0); \

#define raise_stop(condition) \
      do { \
        if (condition) { \
          fprintf(stderr, "[STOP]: %s at %s (%s: %u)\n", #condition, __func__, __FILE__, __LINE__); \
          goto _return; \
        } \
      } while (0); \

#define raise_exception(condition, level, type) \
      do { \
        if (condition) { \
          logger->updateReport(report, exception_type, info, level, type, logger_complete_message(#condition)); \
          if (level > warning) \
            goto _return; \
        } \
      } while (0); \

#define raise_ptr_exception(condition) \
      do { \
        if (condition) { \
          logger->updateReport(report, exception_type, info, error, null_pointer, logger_complete_message(#condition)); \
          goto _return; \
        } \
      } while (0); \

#define handle_exception(exception) \
      do { \
        if (_logger_handle_exception(report, exception, __FILE__, __func__, __LINE__)) \
          goto _return; \
      } while (0); \
      
#define log_custom_status_message(format, param) \
      do { \
        { \
          char complete_message[256]; \
          size_t buffer_size = sizeof(complete_message), \
                 number_characters = snprintf(complete_message, buffer_size, format, param); \
          check_snprintf(logger->dest, number_characters, buffer_size); \
          logger->updateReport(report, status_type, info, none, nil, _logger_complete_message(complete_message, __FILE__, __func__, __LINE__)); \
        } \
      } while (0); \
      
#define log_custom_exception_msg(condition, format, param, level, type) \
      do { \
        if (condition) { \
          char complete_message[256]; \
          size_t buffer_size = sizeof(complete_message), \
                 number_characters = snprintf(complete_message, buffer_size, format, param); \
          check_snprintf(logger->dest, number_characters, buffer_size);  \
          logger->updateReport(report, exception_type, info, level, type, _logger_complete_message(complete_message, __FILE__, __func__, __LINE__)); \
          goto _return; \
        } \
      } while (0); \
      
#define run_log_custom_exception_msg(condition, format, param, level, type) \
      do { \
        if (condition) { \
          char complete_message[256]; \
          size_t buffer_size = sizeof(complete_message), \
                 number_characters = snprintf(complete_message, buffer_size, format, param); \
          check_snprintf(logger.dest, number_characters, buffer_size);  \
          logger.updateReport(report, exception_type, info, level, type, _logger_complete_message(complete_message, __FILE__, __func__, __LINE__)); \
          return_value = type; \
          goto _free; \
        } \
      } while (0); \
      
#define run_raise_exception(condition, level, type) \
      do { \
        if (condition) { \
          logger.updateReport(report, exception_type, info, level, type, logger_complete_message(#condition)); \
          return_value = type; \
          goto _free; \
        } \
      } while (0); \
      
typedef struct {
  User **users;
  size_t s_users;
} Users;

typedef struct {
  size_t max_students_per_classroom;
  size_t max_teachers_per_classroom;
  size_t max_same_class_per_week_per_student;
  size_t max_classes_per_classroom;
  size_t max_blocks_per_day_per_student;
  bool morning_and_afternoon;
} Rules;

void check_snprintf(FILE *logger_file, int number_characters, size_t s_buffer);

bool valid_classes_in_all_classrooms(TimeTable *timetable, size_t max_classes);
bool compare_timetable(void *stateA, void *stateB, void *context);
bool less_than_max_students_per_classroom(size_t max_students_per_classroom, TimeBlock *timeblock, ID class);
bool less_than_max_same_class_per_week_per_student(size_t max_same_class_per_week_per_student, TimeTable *timetable, ID class, ID id);
bool less_than_max_blocks_per_day_per_student(size_t max_blocks_per_day_per_student, TimeTable *timetable, size_t j, ID id);
bool morning_and_afternoon(TimeTable *timetable, Entry *entry, ID id);
bool already_in_class(TimeBlock *timeblock, ID id);
bool in_rules(void *rules, TimeTable *timetable, Entry *entry, ID class, ID user_id, bool is_student);

size_t get_classrooms(ID **available_classrooms, TimeTable *timetable, Entry *block, ID class, size_t max_size);

void _invalid_format(Logger *logger, Report *report, char *file_name, const char *file, const char *func, uint line);

#endif
