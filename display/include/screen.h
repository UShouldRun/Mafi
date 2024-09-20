#ifndef SCREEN_H
#define SCREEN_H

#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "type.h"
#include "error.h"
#include "data.h"

#undef BOOL
#include <curses.h>

#ifndef CHECK_ERROR
#define CHECK_ERROR

#define check_error(error, condition) \
      do { \
        if (condition) { \
          _print_error(error, #condition, __FILE__, __LINE__); \
          return error; \
        } \
      } while (0);

#endif

#ifndef UINT
#define UINT
typedef unsigned int uint;
#endif

#define MAX_TEXT_SIZE 256
typedef struct {
  uint y, x, height, width;
  bool has_colors, has_text, has_command_response;

  double command_response_duration;
  struct timeval timer;

  WINDOW *ptr;

  int text_index;
  char text[MAX_TEXT_SIZE];
} Win;

typedef enum {
  schedule_win = 0,
  user_list_win,
  timetable_list_win,
  log_win,
  stats_win,
  command_win,
  help_win
} DisplayWindows;
#define S_WINDOWS 7

#define USERS_LIT "USERS"
#define TT_LIT "TIMETABLES"
#define SELECTED "-> "
#define FPS_LIT "FPS: "

#define COMMAND ':'
#define COMMAND_DURATION 2.0
#define COMMAND_KEY_SIZE 10

#define HELP "help"
#define HELP_SHORT 'h'
#define HELP_LEN 4

#define QUIT "quit"
#define QUIT_SHORT 'q'
#define QUIT_LEN 4

#define SET "set"
#define SET_SHORT 's'
#define SET_LEN 3
#define SET_USER "user"
#define SET_USER_LEN 4
#define SET_TIMETABLE "timetable"
#define SET_TIMETABLE_LEN 9
#define SET_OFFSET "offset"
#define SET_OFFSET_LEN 6

#define RUN "run"
#define RUN_SHORT 'r'
#define RUN_LEN 3

#define OPEN "open"
#define OPEN_SHORT 'o'
#define OPEN_LEN 3

#define MAFI "mafi"
#define MAFI_SHORT 'm'
#define MAFI_LEN 4

#define ERROR_COMMAND_NO_USER_MATCHED "command error: no user matched"
#define ERROR_COMMAND_SIZE "command error: instruction size was too long (max size: 256 characters)"
#define ERROR_COMMAND_INVALID "command error: invalid instruction (type :h or :help to know more)"

Error init_windows(Win windows[S_WINDOWS], uint screen_width, uint screen_height, bool colors_on);

bool is_digit(char ch);
size_t len_id(ID id);

bool is_valid_user(ID selected_user, ID *users, size_t s_users);

char peek(char *str, size_t index, size_t len_str);
void strcut(char *buffer, char *str, size_t start, size_t len_buffer, size_t len_str);
bool parse_word(char *str, const char *word, size_t len_str, const size_t len_word, size_t start);

bool get_command(Win *window);
int parse_command(
  Win *window, bool parse,
  FILE **file, bool *help_on, 
  size_t *offset_timetable, size_t *offset_user,
  size_t *current_timetable, ID *current_user,
  TimeTable **timetables, ID **users,
  size_t *s_timetables, size_t *s_users
);
void window_erase_text(Win *window);
void backspace_erase(Win *window);
void print_command_text(Win *window);
void print_command_response(Win *window, const char *msg);
void handle_command_response(Win *window, struct timeval *current_time);

Error display_timetables(Win *window, size_t selected_timetable, size_t s_timetables, size_t offset);
Error display_users(Win *window, ID selected_user, ID *users, size_t s_users, size_t offset);

void draw_grid(WINDOW *win, int num_rows, int num_cols);
Error display_user_schedule(Win *window, TimeTable *timetable, ID user);

void update_fps(Win *fps_win, struct timeval *current_time, struct timeval *last_sec, uint *frame_counter);
Error refresh_windows(
  Win windows[S_WINDOWS], bool help_on, size_t offset_timetables, size_t offset_user,
  struct timeval *current_time, struct timeval *last_refresh,
  uint *frame_counter, const double frame_time,
  size_t current_timetable, TimeTable *timetables, size_t s_timetables,
  ID current_user, ID *users, size_t s_users
);

#endif
