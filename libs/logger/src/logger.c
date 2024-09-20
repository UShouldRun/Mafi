#include "logger.h"

// =========================================================# PUBLIC #==============================================================

// Logger Write 
void logger_write_message(Message *msg, FILE *dest, Type t, StatusLevel s_level, ExceptionLevel e_level, const char *mark, size_t offset) {
  if (!msg)
		return;
  if (!dest)
		return;

  uint mark_printed = 0;
  for (LLMessage *node = msg->list; node; node = node->next) {
    if (!node->track)
      continue;
    if (strncmp(node->track, "\n", 1) == 0)
      continue;

    char *place_holder_position = strstr(node->track, "%s");
    if (!place_holder_position) {
      fprintf(dest, "%*s%s ...\n", (node == msg->list ? 0 : (int)offset) + 1, "", node->track);
      continue;
    }

    size_t buffer_size = strlen(node->track) + 1;
    if (!buffer_size) {
      fprintf(
        stderr, "[ERROR]: [%s] buffer_size == 0 at %s (%s: %u)\n",
        _logger_match_exception_type(unexp_behaviour), __func__, __FILE__, __LINE__
      );
      return;
    }

    char *formatted_msg_track = (char *)malloc(buffer_size * sizeof(char));
    if (!formatted_msg_track) {
      fprintf(
        stderr, "[ERROR]: [%s] formatted_msg_track == NULL at %s (%s: %u)\n",
        _logger_match_exception_type(unexp_behaviour), __func__, __FILE__, __LINE__
      );
      return;
    }

    snprintf(
      formatted_msg_track, buffer_size,
      node->track, node->type == status_type ? "->" : (node->type != status_type && !mark_printed ? "at" : "by")
    );

    if (node->type != status_type && !mark_printed) {
      fprintf(dest, "%s%s%*s%s ...\n", _logger_match_colors(node->type, s_level, e_level), mark, 1, "", formatted_msg_track);
      mark_printed++;
    } else {
      fprintf(
        dest, "%s%s%*s%s ...\n", _logger_match_colors(node->type, s_level, e_level),
        (t == status_type && node == msg->list ? mark : ""), (t == status_type && node == msg->list ? 0 : (int)offset) + 1, "",
        formatted_msg_track
      );
    }

    free(formatted_msg_track);
  }

  fprintf(dest, "%s", _logger_colors[0]);
}

void logger_write_info(Message *msg, FILE *dest) {
  if (!msg)
		return;
  if (!dest)
		return;

  const char *info_mark = _logger_match_status_level(info);
  logger_write_message(msg, dest, status_type, info, none, info_mark, strlen(info_mark));
}

void logger_write_debug(Message *msg, FILE *dest) {
  if (!msg)
		return;
  if (!dest)
		return;

  const char *debug_mark = _logger_match_status_level(debug);
  logger_write_message(msg, dest, status_type, debug, none, debug_mark, strlen(debug_mark));
  fprintf(dest, "%s", _logger_colors[RESET]);
}

void logger_write_trace(Message *msg, FILE *dest) {
  if (!msg)
		return;
  if (!dest)
		return;

  const char *trace_mark = _logger_match_status_level(trace);
  logger_write_message(msg, dest, status_type, trace, none, trace_mark, strlen(trace_mark));
  fprintf(dest, "%s", _logger_colors[RESET]);
}

void logger_write_perf(Message *msg, FILE *dest) {
  if (!msg)
		return;
  if (!dest)
		return;

  const char *perf_mark = _logger_match_status_level(perf);
  logger_write_message(msg, dest, status_type, perf, none, perf_mark, strlen(perf_mark));
  fprintf(dest, "%s", _logger_colors[RESET]);
}

void logger_write_warning(Exception *except, FILE *dest) {
  if (!except)
		return;
  if (!dest)
		return;

  const char *warning_mark = _logger_match_exception_level(warning),
             *type_mark = _logger_match_exception_type(except->type);
  size_t buffer_size = (strlen(warning_mark) + strlen(type_mark) + 3) * sizeof(char);

  char *exception_mark = (char *)malloc(buffer_size);
  if (!exception_mark) {
    fprintf(stderr, "[ERROR]: [%s] exception_mark == NULL at %s (%s: %u)\n", _logger_match_exception_type(null_pointer), __func__, __FILE__, __LINE__);
    return;
  }

  snprintf(exception_mark, buffer_size, "%s[%s]", warning_mark, type_mark);

  logger_write_message(&except->msg, dest, exception_type, info, warning, exception_mark, strlen(exception_mark));
  fprintf(dest, "%s", _logger_colors[RESET]);

  free(exception_mark);
}

void logger_write_error(Exception *except, FILE *dest) {
  if (!except)
		return;
  if (!dest)
		return;

  const char *error_mark = _logger_match_exception_level(error),
             *type_mark = _logger_match_exception_type(except->type);
  size_t buffer_size = (strlen(_logger_colors[RED]) + strlen(error_mark) + strlen(type_mark) + 3) * sizeof(char);
  char *exception_mark = (char *)malloc(buffer_size);
  if (!exception_mark) {
    fprintf(stderr, "[ERROR]: [%s] exception_mark == NULL at %s (%s: %u)\n", _logger_match_exception_type(null_pointer), __func__, __FILE__, __LINE__);
    return;
  }

  snprintf(exception_mark, buffer_size, "%s[%s]", error_mark, type_mark);

  logger_write_message(&except->msg, dest, exception_type, info, error, exception_mark, strlen(exception_mark));
  fprintf(dest, "%s", _logger_colors[RESET]);

  free(exception_mark);
}

void logger_write_fatal(Exception *except, FILE *dest) {
  if (!except)
		return;
  if (!dest)
		return;

  const char *fatal_mark = _logger_match_exception_level(fatal),
             *type_mark = _logger_match_exception_type(except->type);
  size_t buffer_size = (strlen(_logger_colors[RED]) + strlen(fatal_mark) + strlen(type_mark) + 3) * sizeof(char);
  char *exception_mark = (char *)malloc(buffer_size);
  if (!exception_mark) {
    fprintf(stderr, "[ERROR]: [%s] exception_mark == NULL at %s (%s: %u)\n", _logger_match_exception_type(null_pointer), __func__, __FILE__, __LINE__);
    return;
  }

  snprintf(exception_mark, buffer_size, "%s[%s]", fatal_mark, type_mark);
  
  logger_write_message(&except->msg, dest, exception_type, info, fatal, exception_mark, strlen(exception_mark));

  free(exception_mark);

  exit(except->type);
}

void logger_write_status(Status *status, FILE *dest) {
  if (!status)
		return;
  if (!dest)
		return;

  switch (status->level) {
    case info:
      logger_write_info(&status->msg, dest);
      return;
    case debug:
      logger_write_debug(&status->msg, dest);
      return;
    case trace:
      logger_write_trace(&status->msg, dest);
      return;
    case perf:
      logger_write_perf(&status->msg, dest);
      return;
    default:
      return;
  }
}

void logger_write_exception(Exception *except, FILE *dest) {
  if (!except)
		return;
  if (!dest)
		return;

  switch (except->level) {
    case warning:
      logger_write_warning(except, dest);
      return;
    case error:
      logger_write_error(except, dest); 
      return;
    case fatal:
      logger_write_fatal(except, dest);
      return;
    default:
      return;
  }
}

void logger_write_report(Report *report, FILE *dest) {
  if (!report)
		return;
  if (!dest)
		return;

  switch (report->type) {
    case status_type:
      logger_write_status(&report->result.status, dest);
      return;
    case exception_type:
      logger_write_exception(&report->result.exception, dest);
      return;
    default:
      return;
  }
}

void logger_write_log(void *log, Type t, FILE *dest) {
  if (!log)
		return;
  if (!dest)
		return;

  switch (t) {
    case status_type:
      logger_write_status((Status *)log, dest);
      return;
    case exception_type:
      logger_write_exception((Exception *)log, dest);
      return;
    case report_type:
      logger_write_report((Report *)log, dest);
      return;
    default:
      return;
  }
}

// Logger Create
Status *logger_create_status(StatusLevel status_level) {
  Status *status = (Status *)malloc(sizeof(Status));
  if (!status)
    return NULL;
  status->level = status_level;
  status->msg.len = 0;
  status->msg.list = NULL;
  return status;
}

Exception *logger_create_exception(ExceptionLevel exception_level, ExceptionType _exception_type) {
  Exception *exception = (Exception *)malloc(sizeof(Exception));
  if (!exception)
    return NULL;
  exception->level = exception_level;
  exception->type = exception_type;
  exception->msg.len = 0;
  exception->msg.list = NULL;
  return exception;
}

Report *logger_create_report(Type type, StatusLevel status_level, ExceptionLevel exception_level, ExceptionType _exception_type) {
  Report *report = (Report *)malloc(sizeof(Report));
  if (!report)
    return NULL;
  report->type = type;
  
  switch (type) {
    case status_type:
      report->result.status.level = status_level;
      report->result.status.msg.len = 0;
      report->result.status.msg.list = NULL;
      break;

    case exception_type:
      report->result.exception.level = exception_level;
      report->result.exception.type = exception_type;
      report->result.exception.msg.len = 0;
      report->result.exception.msg.list = NULL;
      break;

    default:
      return NULL;
  }

  return report;
}

// Update
void logger_update_status(Status *status, StatusLevel status_level, char *update) {
  status->level = status_level;

  Message *msg = logger_append_to_message_track(&status->msg, update, status_type);
  if (msg) {
    status->msg.len = msg->len;
    status->msg.list = msg->list;
    free(msg);
  }
}

void logger_update_exception(Exception *exception, ExceptionLevel exception_level, ExceptionType _exception_type, char *update) {
  exception->level = exception_level;
  exception->type = _exception_type;

  Message *msg = logger_append_to_message_track(&exception->msg, update, exception_type);
  if (msg) {
    exception->msg.len = msg->len;
    exception->msg.list = msg->list;
    free(msg);
  }
}

void logger_update_report(
  Report *report, Type type, StatusLevel status_level, ExceptionLevel exception_level, ExceptionType _exception_type, char *update
) {
  report->type = type;
  switch (type) {
    case status_type:
      logger_update_status(&report->result.status, status_level, update);
      return;
    case exception_type:
      logger_update_exception(&report->result.exception, exception_level, _exception_type, update);
      return;
    default:
      return;
  }
}

void logger_update_report_with_status(Report *report, Status *status) {
  Message *new_msg = NULL;

  switch (report->type) {
    case status_type:
      new_msg = logger_join_message_track(&report->result.status.msg, &status->msg);
      if (!new_msg)
        return;
      report->result.status.level = _logger_merge_status_level(report->result.status.level, status->level);
      report->result.status.msg.len = new_msg->len;
      report->result.exception.msg.list = new_msg->list;
      break;

    case exception_type:
      new_msg = logger_join_message_track(&report->result.exception.msg, &status->msg);
      if (!new_msg)
        return;
      report->result.exception.msg.len = new_msg->len;
      report->result.exception.msg.list = new_msg->list;
    break;

    default:
      return;
  }

  free(new_msg);
}

void logger_update_report_with_exception(Report *report, Exception *exception) {
  Message *new_msg = NULL;

  switch (report->type) {
    case status_type:
      new_msg = logger_join_message_track(&report->result.status.msg, &exception->msg);
      if (!new_msg)
        return;
      report->type = exception_type;
      report->result.exception.level = exception->level;
      report->result.exception.type = exception->type;
      report->result.exception.msg.len = new_msg->len;
      report->result.exception.msg.list = new_msg->list;
      break;

    case exception_type:
      new_msg = logger_join_message_track(&report->result.exception.msg, &exception->msg);
      if (!new_msg)
        return;
      report->result.exception.level = _logger_merge_exception_level(report->result.exception.level, exception->level);
      report->result.exception.type = _logger_merge_exception_type(report->result.exception.type, exception->type);
      report->result.exception.msg.len = new_msg->len;
      report->result.exception.msg.list = new_msg->list;
      break;

    default:
      return;
  }

  free(new_msg);
}

void *logger_update_log(void *log, Type t, char *update) {
  Status *status;
  Exception *exception;
  Report *report;
  Message *msg;

  switch (t) {
    case status_type:
      status = (Status *)log;

      msg = logger_append_to_message_track(&status->msg, update, t);
      if (msg) {
        status->msg = *msg;
        free(msg);
      }
      else {
        status->msg.len = 0;
        status->msg.list = NULL;
      }

      return (void *)status;

    case exception_type:
      exception = (Exception *)log;

      msg = logger_append_to_message_track(&exception->msg, update, t);
      if (msg) {
        exception->msg = *msg;
        free(msg);
      }
      else {
        exception->msg.len = 0;
        exception->msg.list = NULL;
      }

      return (void *)exception;

    case report_type:
      report = (Report *)log;

      switch (report->type) {
        case status_type:
          status = (Status *)logger_update_log(&report->result.status, status_type, update);
          if (status)
            report->result.status = *status;
          else {
            report->result.status.level = info;
            report->result.status.msg.len = 0;
            report->result.status.msg.list = NULL;
          }
          break;

        case exception_type:
          exception = (Exception *)logger_update_log(&report->result.exception, exception_type, update);
          if (exception)
            report->result.exception = *exception;
          else {
            report->result.exception.level = none;
            report->result.exception.type = nil;
            report->result.exception.msg.len = 0;
            report->result.exception.msg.list = NULL;
          }
          break;

        default:
          report = NULL;
      }

      return (void *)report;

    default:
      return NULL;
  }
}

// Logger Delete Log
void logger_delete_status(Status *status) {
  if (!status)
    return;
  logger_delete_message_track(status->msg.list);
  free(status);
}

void logger_delete_exception(Exception *exception) {
  if (!exception)
    return;
  logger_delete_message_track(exception->msg.list);
  free(exception);
}

void logger_delete_report(Report *report) {
  if (!report)
    return;

  switch (report->type) {
    case status_type:
      logger_delete_message_track(report->result.status.msg.list);
      break;
    case exception_type:
      logger_delete_message_track(report->result.exception.msg.list);
      break;
    default:
      break;
  }

  free(report);
}

void logger_delete_log(void *log, Type t) {
  if (!log)
    return;

  Status *status = NULL;
  Exception *exception = NULL;
  Report *report = NULL;

  switch (t) {
    case status_type:
      status = (Status *)log;
      logger_delete_message_track(status->msg.list);
      free(status);
      return;

    case exception_type:
      exception = (Exception *)log;
      logger_delete_message_track(exception->msg.list);
      free(exception);
      return;

    case report_type:
      report = (Report *)log;

      switch (report->type) {
        case status_type:
          logger_delete_message_track(report->result.status.msg.list);
          break;
        case exception_type:
          logger_delete_message_track(report->result.exception.msg.list);
          break;
        default:
          break;
      }

      free(report);
      return;

    default:
      return;
  }
}

// Logger Message
size_t logger_get_len_message_track(Message *msg) {
  size_t len = 0;
  for (LLMessage *node = msg->list; node; node = node->next, len++);
  return len;
}

Message *logger_append_to_message_track(Message *msg, char *segment, Type type) {
  if (!segment)
    return msg;

  Message *new_msg = (Message *)malloc(sizeof(Message));
  if (!new_msg)
    return NULL;

  LLMessage *new_node = (LLMessage *)malloc(sizeof(LLMessage));
  if (!new_node) {
    free(new_msg);
    return NULL;
  }

  new_node->track = segment;
  new_node->type = type;
  new_node->next = NULL;

  if (!msg) {
    new_msg->list = new_node;
    new_msg->len = 1;
    return new_msg;
  }
  if (!msg->list) {
    new_msg->list = new_node;
    new_msg->len = 1;
    return new_msg;
  }

  new_msg->list = msg->list;
  LLMessage *node = msg->list;
  for (; node->next; node = node->next);
  node->next = new_node;

  new_msg->len = msg->len + 1;
  return new_msg;
}

Message *logger_join_message_track(Message *msgA, Message *msgB) {
  if (!msgA)
    return msgB;
  if (!msgB)
    return msgA;
  if (!msgA->list)
    return msgB;
  if (!msgB->list)
    return msgA;

  Message *new_msg = (Message *)malloc(sizeof(Message));
  if (!new_msg)
    return NULL;

  new_msg->len = msgA->len + msgB->len + 1;
  new_msg->list = msgA->list;

  LLMessage *node = new_msg->list;
  for (; node->next; node = node->next);

  LLMessage *linker = (LLMessage *)malloc(sizeof(LLMessage));
  if (!linker) {
    free(new_msg);
    return NULL;
  }

  linker->track = strdup("\n");
  if (!linker->track) {
    free(linker);
    free(new_msg);
    return NULL;
  }

  linker->type = status_type;
  linker->next = msgB->list;
  node->next = linker; 

  msgA->list = NULL;
  msgB->list = NULL;
  
  return new_msg;
}

Message *logger_insert_message_track(Message *msg, char *segment, Type type, size_t step) {
  if (!segment)
		return msg;

  Message *new_msg = (Message *)malloc(sizeof(Message));
  if (!new_msg)
    return NULL;
  new_msg->len = (msg ? msg->len : 0) + 1;
  new_msg->list = msg->list;

  LLMessage *node = (LLMessage *)malloc(sizeof(LLMessage));
  if (!node) {
    free(new_msg);
		return NULL;
  }
  node->track = segment;
  node->type = type;
  node->next = NULL;

  if (!new_msg->list) {
    new_msg->list = node;
    return new_msg;
  }

  LLMessage *walk = new_msg->list;
  for (size_t i = 0; walk; walk = walk->next, i++) {
    if (i < step - 1) continue;
    node->next = walk->next;
    walk->next = node;
    break;
  }

  return new_msg;
}

Message *logger_replace_message_track(Message *msg, char *segment, size_t step) {
  if (!segment)
		return msg;
  if (!msg)
		return NULL;
  if (step >= msg->len)
		return msg;

  Message *new_msg = (Message *)malloc(sizeof(Message));
  if (!new_msg)
    return NULL;
  new_msg->len = msg->len;
  new_msg->list = msg->list;

  LLMessage *walk = new_msg->list;
  for (size_t i = 0; i < step && walk; i++, walk = walk->next);
  if (walk) {
    free(walk->track);
    walk->track = segment;
  }

  return new_msg;
}

void logger_delete_message_track(LLMessage *node) {
  LLMessage *next = NULL;
  while (node) {
    next = node->next;
    if (node->track)
      free(node->track);
    free(node);
    node = next;
  }
}

void logger_delete_message(Message *msg) {
  if (!msg)
    return;
  logger_delete_message_track(msg->list);
  free(msg);
}

void initLogger(Logger *logger, FILE *dest) {
  // Logger File
  logger->dest = dest;

  // Logger Write
  logger->writeMessage = logger_write_message;
  
  logger->writeInfo = logger_write_info;
  logger->writeDebug = logger_write_debug;
  logger->writeTrace = logger_write_trace;
  logger->writePerf = logger_write_perf;

  logger->writeWarning = logger_write_warning;
  logger->writeError = logger_write_error;
  logger->writeFatal = logger_write_fatal;

  logger->writeStatus = logger_write_status;
  logger->writeException = logger_write_exception;
  logger->writeReport = logger_write_report;
  logger->writeLog = logger_write_log;

  // Logger Create
  logger->createStatus = logger_create_status;
  logger->createException = logger_create_exception;
  logger->createReport = logger_create_report;

  // Logger Update
  logger->updateStatus = logger_update_status;
  logger->updateException = logger_update_exception;
  logger->updateReport = logger_update_report;

  logger->updateReportWithStatus = logger_update_report_with_status;
  logger->updateReportWithException = logger_update_report_with_exception;

  logger->updateLog = logger_update_log;

  // Logger Delete Log
  logger->deleteStatus = logger_delete_status;
  logger->deleteException = logger_delete_exception;
  logger->deleteReport = logger_delete_report;
  logger->deleteLog = logger_delete_log;
  
  // Logger Message
  logger->getLenMessageTrack = logger_get_len_message_track;
  logger->appendToMessageTrack = logger_append_to_message_track;
  logger->joinMessageTrack = logger_join_message_track;
  logger->insertMessageTrack = logger_insert_message_track;
  logger->replaceMessageTrack = logger_replace_message_track;
  logger->deleteMessageTrack = logger_delete_message_track;
  logger->deleteMessage = logger_delete_message;
}

// =========================================================# PRIVATE #==============================================================

// Logger Message Completion and Exception Handling
char *_logger_complete_message(const char *msg, const char *file, const char *func, uint line) {
  size_t buffer_size = snprintf(NULL, 0, "%s %%s %s (%s: %u)\n", msg, func, file, line) + 1;
  
  char *complete_message = (char *)malloc(buffer_size);
  if (!complete_message)
    return NULL;

  size_t number_characters = snprintf(
    complete_message, buffer_size, "%s %%s %s (%s: %u)", msg, func, file, line
  );
  if (number_characters >= buffer_size)
    logger_panic(stderr, "Buffer char complete_message too small", number_characters);
  else if (number_characters < 0)
    logger_panic(stderr, "snprintf failed with encoding error or other issue", number_characters);

  return complete_message;
}

int _logger_handle_exception(Report *report, Exception *exception, const char *file, const char *func, uint line) {
  if (!report)
    return -1;
  if (!exception) {
    logger_update_report(
      report, exception_type, info, error, null_pointer,
     _logger_complete_message("NULL pointer Exception *exception", file, func, line)
    );
    return 1;
  }
  if (exception->level != none) {
    logger_update_report_with_exception(report, exception);
    return 2;
  }
  return 0;
}

void _logger_panic(FILE *logger_file, const char *panic_message, int exception, const char *file, const char *func, uint line) {
  fprintf(
      logger_file, "%s[FATAL]:\n\t%s (exception value: %d) at %s (%s: %u)%s\n",
      _logger_colors[RED], panic_message, exception, func, file, line, _logger_colors[RESET]
    );
  exit(exception);
}

// Logger Level and Type Helper Functions
StatusLevel _logger_merge_status_level(StatusLevel levelA, StatusLevel levelB) {
  return levelA > levelB ? levelA : levelB;
}

ExceptionLevel _logger_merge_exception_level(ExceptionLevel levelA, ExceptionLevel levelB) {
  return levelA > levelB ? levelA : levelB;
}

ExceptionType _logger_merge_exception_type(ExceptionType typeA, ExceptionType typeB) {
  return typeA > typeB ? typeA : typeB;
}

const char *_logger_match_status_level(StatusLevel level) {
  return _logger_status_levels[level];
}

const char *_logger_match_exception_level(ExceptionLevel level) {
  return _logger_exception_levels[level];
}

const char *_logger_match_exception_type(ExceptionType type) {
  return _logger_exception_types[type];
}

const char *_logger_match_colors(Type type, StatusLevel s_level, ExceptionLevel e_level) {
  switch (type) {
    case status_type: {
      switch (s_level) {
        case info:        return _logger_colors[2];
        case debug:       return _logger_colors[6];
        case trace:       return _logger_colors[4];
        case perf:        return _logger_colors[7];
        default:          return _logger_colors[0];
      }
    }
    case exception_type: {
      switch (e_level) {
        case warning:     return _logger_colors[3];
        case error:       return _logger_colors[1];
        case fatal:       return _logger_colors[5];
        default:          return _logger_colors[0];
      }
    }
    default: {
      return _logger_colors[0];
    }
  }
}
