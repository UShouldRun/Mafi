#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================# PUBLIC #==============================================================

typedef enum {
  info = 0,
  debug,
  trace,
  audit,
  perf
} StatusLevel;

typedef enum {
  none = 0,
  warning,
  error,
  fatal
} ExceptionLevel;

typedef enum {
  nil = 0,
  invalid_argument,
  index_out_of_bounds,
  null_pointer,
  file_not_found,
  io_error,
  mem_error,
  unexp_behaviour,
  unknown_error
} ExceptionType;

typedef enum {
  status_type = 0,
  exception_type,
  report_type
} Type;

typedef struct msg_node {
  char *track;
  Type type;
  struct msg_node *next;
} LLMessage;

typedef struct {
  size_t len;
  LLMessage *list;
} Message;

typedef struct {
  StatusLevel level;
  Message msg;
} Status;

typedef struct {
  ExceptionLevel level;
  ExceptionType type;
  Message msg;
} Exception;

typedef union {
  Status status;
  Exception exception;
} ReportUnion;

typedef struct {
  Type type;
  ReportUnion result;
} Report;

typedef struct {
  // Logger File
  FILE *dest;

  // Logger Write
  void (*writeMessage)(Message *msg, FILE *dest, Type t, StatusLevel s_level, ExceptionLevel e_level, const char *mark, size_t offset);
  void (*writeInfo)(Message *msg, FILE *dest);
  void (*writeDebug)(Message *msg, FILE *dest);
  void (*writeTrace)(Message *msg, FILE *dest);
  void (*writePerf)(Message *msg, FILE *dest);
  void (*writeWarning)(Exception *exception, FILE *dest);
  void (*writeError)(Exception *exception, FILE *dest);
  void (*writeFatal)(Exception *exception, FILE *dest);
  void (*writeStatus)(Status *status, FILE *dest);
  void (*writeException)(Exception *exception, FILE *dest);
  void (*writeReport)(Report *report, FILE *dest);
  void (*writeLog)(void *log, Type t, FILE *dest);

  // Logger Create
  Status *(*createStatus)(StatusLevel status_level);
  Exception *(*createException)(ExceptionLevel exception_level, ExceptionType _exception_type);
  Report *(*createReport)(Type type, StatusLevel status_level, ExceptionLevel exception_level, ExceptionType _exception_type);

  // Logger Update
  void (*updateStatus)(Status *status, StatusLevel status_level, char *update);
  void (*updateException)(Exception *exception, ExceptionLevel exception_level, ExceptionType _exception_type, char *update);

  void (*updateReport)(
    Report *report, Type type, StatusLevel status_level, ExceptionLevel exception_level, ExceptionType _exception_type, char *update
  );
  void (*updateReportWithStatus)(Report *report, Status *status);
  void (*updateReportWithException)(Report *report, Exception *exception);

  void *(*updateLog)(void *log, Type t, char *update);

  // Logger Delete Log
  void (*deleteStatus)(Status *status);
  void (*deleteException)(Exception *exception);
  void (*deleteReport)(Report *report);
  void (*deleteLog)(void *log, Type t);

  // Logger Message
  size_t (*getLenMessageTrack)(Message *msg);

  Message *(*joinMessageTrack)(Message *msgA, Message *msgB);
  Message *(*appendToMessageTrack)(Message *msg, char *segment, Type type);
  Message *(*insertMessageTrack)(Message *msg, char *segment, Type type, size_t step);
  Message *(*replaceMessageTrack)(Message *msg, char *segment, size_t step);

  void (*deleteMessageTrack)(LLMessage *node);
  void (*deleteMessage)(Message *msg);

} Logger;

// Logger Write 
void logger_write_message(Message *msg, FILE *dest, Type t, StatusLevel s_level, ExceptionLevel e_level, const char *mark, size_t offset);

void logger_write_info(Message *msg, FILE *dest);
void logger_write_debug(Message *msg, FILE *dest);
void logger_write_trace(Message *msg, FILE *dest);
void logger_write_perf(Message *msg, FILE *dest);

void logger_write_warning(Exception *exception, FILE *dest);
void logger_write_error(Exception *exception, FILE *dest);
void logger_write_fatal(Exception *exception, FILE *dest);

void logger_write_status(Status *status, FILE *dest);
void logger_write_exception(Exception *exception, FILE *dest);
void logger_write_report(Report *report, FILE *dest);
void logger_write_log(void *log, Type t, FILE *dest);

// Logger Create
Status *logger_create_status(StatusLevel status_level);
Exception *logger_create_exception(ExceptionLevel exception_level, ExceptionType _exception_type);
Report *logger_create_report(Type type, StatusLevel status_level, ExceptionLevel exception_level, ExceptionType _exception_type);

// Logger Update
void logger_update_status(Status *status, StatusLevel status_level, char *update);
void logger_update_exception(Exception *exception, ExceptionLevel exception_level, ExceptionType _exception_type, char *update);
void logger_update_report(
  Report *report, Type type, StatusLevel status_level, ExceptionLevel exception_level, ExceptionType _exception_type, char *update
);

void logger_update_report_with_status(Report *report, Status *status);
void logger_update_report_with_exception(Report *report, Exception *exception);

void *logger_update_log(void *log, Type t, char *update);

// # PRIVATE # Logger Message Completion and Exception Handling
char *_logger_complete_message(const char *msg, const char *file, const char *func, uint line);
int _logger_handle_exception(Report *report, Exception *exception, const char *file, const char *func, uint line);
void _logger_panic(FILE *logger_file, const char *panic_message, int exception, const char *file, const char *func, uint line);

// # PUBLIC # Logger Message Completion and Exception Handling
#define logger_complete_message(message) _logger_complete_message(message, __FILE__, __func__, __LINE__)
#define logger_handle_exception(report, exception) _logger_handle_exception(report, exception, __FILE__, __func__, __LINE__)
#define logger_panic(logger_file, panic_message, exception) _logger_panic(logger_file, panic_message, exception, __FILE__, __func__, __LINE__)

#define logger_update_report_status(logger, report, level, msg) \
  logger->updateReport(report, status_type, level, none, nil, _logger_complete_message(msg, __FILE__, __func__, __LINE__));

#define logger_update_report_exception(logger, report, level, type, msg) \
  logger->updateReport(report, exception_type, info, level, type, _logger_complete_message(msg, __FILE__, __func__, __LINE__));

// Logger Delete Log
void logger_delete_status(Status *status);
void logger_delete_exception(Exception *exception);
void logger_delete_report(Report *report);
void logger_delete_log(void *log, Type t);

// Logger Message
size_t logger_get_len_message_track(Message *msg);
Message *logger_join_message_track(Message *msgA, Message *msgB);
Message *logger_append_to_message_track(Message *msg, char *segment, Type type);
Message *logger_insert_message_track(Message *msg, char *segment, Type type, size_t step);
Message *logger_replace_message_track(Message *msg, char *segment, size_t step);
void logger_delete_message_track(LLMessage *node);
void logger_delete_message(Message *msg);

void initLogger(Logger *logger, FILE *dest);

// =========================================================# PRIVATE #==============================================================

// Logger Global Variables and Enums

typedef enum {
  RESET = 0,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE 
} Colors;

static const char _logger_colors[8][10] = {
  "\033[0m",  // Reset
  "\033[31m", // Red
  "\033[32m", // Green
  "\033[33m", // Yellow
  "\033[34m", // Blue
  "\033[35m", // Magenta
  "\033[36m", // Cyan
  "\033[37m", // White
};

static const char _logger_status_levels[5][20] = {
  "[INFO]: ",
  "[DEBUG]: ",
  "[TRACE]: ",
  "[AUDIT]: ",
  "[PERF]: "
};

static const char _logger_exception_levels[4][20] = {
  "[none]: ",
  "[WARNING]: ",
  "[ERROR]: ",
  "[FATAL]: "
};

static const char _logger_exception_types[9][21] = {
  "nil",
  "Invalid Argument",
  "Index Out of Bounds",
  "Null Pointer",
  "File Not Found",
  "IO Error",
  "Memory Error",
  "Unexpected Behaviour",
  "Unknown Error"
};

// Level and Type Helper functions
StatusLevel _logger_merge_status_level(StatusLevel levelA, StatusLevel levelB);
ExceptionLevel _logger_merge_exception_level(ExceptionLevel levelA, ExceptionLevel levelB);
ExceptionType _logger_merge_exception_type(ExceptionType typeA, ExceptionType typeB);

const char *_logger_match_status_level(StatusLevel level);
const char *_logger_match_exception_level(ExceptionLevel level);
const char *_logger_match_exception_type(ExceptionType type);

const char *_logger_match_colors(Type type, StatusLevel s_level, ExceptionLevel e_level);

#endif // LOGGER_H
