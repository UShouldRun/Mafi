#include "data.h"
#include "screen.h"

#define check_ret(error, condition) \
      do { \
        if (condition) { \
          _print_error(error, #condition, __FILE__, __LINE__); \
          ret = error; \
          goto _return; \
        } \
      } while (0);

int main(int argc, char *argv[]) {
  Error ret = none;

  check_ret(inval_arg, argc < 2);

  size_t s_timetables = 0;
  size_t s_users = 0;
  TimeTable *timetables = NULL; 
  ID *users = NULL;

  size_t current_timetable = 0;
  ID current_user;

  size_t offset_timetable = 0, offset_user = 0;

  FILE *file = fopen(argv[1], "r");
  check_ret(ioerror, file == NULL);
  Error read_error = read_mw(file, &timetables, &s_timetables);
  check_ret(read_error, read_error != none);

  s_users = get_users(&timetables[current_timetable], &users);
  check_ret(nullptr, users == NULL);
  current_user = users[0];

  initscr();
  bool init = true;
  
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  
  const uint fps = 120;
  const double frame_time = 1.0 / fps;
  uint frame_counter = 0;

  const bool colors_on = has_colors(), can_change = can_change_color();
  if (colors_on && can_change) {
    start_color();

    init_color(COLOR_BLACK, 120, 120, 120);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    init_color(COLOR_CYAN, 200, 200, 200);
    init_pair(2, COLOR_WHITE, COLOR_CYAN);
  }

  uint screen_height, screen_width;
  getmaxyx(stdscr, screen_height, screen_width);

  Win windows[S_WINDOWS];
  check_ret(unexp_behaviour, init_windows(windows, screen_width, screen_height, colors_on) != none);
  bool help_on = false;
 
  struct timeval last_refresh, last_sec, current_time;
  gettimeofday(&last_refresh, NULL);
  gettimeofday(&last_sec, NULL);

  bool running = true;
  nodelay(windows[command_win].ptr, TRUE);

  while (running) {
    bool parse = get_command(&windows[command_win]);

    int command = parse_command(
      &windows[command_win], parse,
      &file, &help_on,
      &offset_timetable, &offset_user,
      &current_timetable, &current_user,
      &timetables, &users,
      &s_timetables, &s_users
    );

    if (command == 1)
      running = false;
    
    gettimeofday(&current_time, NULL);
    update_fps(&windows[stats_win], &current_time, &last_sec, &frame_counter); 
    check_ret(
      unexp_behaviour,
      refresh_windows(
        windows, help_on, offset_timetable, offset_user,
        &current_time, &last_refresh, &frame_counter, frame_time,
        current_timetable, timetables, s_timetables,
        current_user, users, s_users
      ) != none
    ); 
  }

_return:
  if (users)
    free(users);
  if (timetables) {
    for (size_t k = 0; k < s_timetables; k++) {
      for (size_t i = 0; i < timetables[k].n; i++)
        type_delete_timeblock(&timetables[k].entries[i], timetables[k].m);
      free(timetables[k].entries);
    }
    free(timetables);
  }
  if (file)
    fclose(file);

  if (init)
    endwin();

  print_exit_value(ret);
  return (int)ret;
}
