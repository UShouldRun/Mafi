#include "screen.h"

static const char *help_text = "\tTo QUIT the help window type :h or :help again\n\n\tDisplay Commands:\n\t-> :h or :help to open the help window\n\t-> :q or :quit to close the program\n\t-> :s user/timetable <id>/<index> or :set user/timetable <id>/<index> to set a new user or a new timetable\n\t-> :o <.mw file> or :open <.mw file> to open a new mafi write file\n\t-> :m <mafi input params> or -mafi <mafi input params> to run mafi inside display\n";

Error init_windows(Win windows[S_WINDOWS], uint screen_width, uint screen_height, bool colors_on) {
  Error ret = none;

  windows[schedule_win].y          = 0;
  windows[schedule_win].x          = 0;
  windows[schedule_win].height     = screen_height - screen_height / 24;
  windows[schedule_win].width      = 7 * screen_width / 8;
  windows[schedule_win].has_colors = colors_on;
  windows[schedule_win].has_text = false;
  windows[schedule_win].has_command_response = false;
  windows[schedule_win].command_response_duration = 0;
  windows[schedule_win].ptr        = NULL;
  windows[schedule_win].text_index = -1;
  windows[schedule_win].text[0] = '\0';

  windows[user_list_win].y          = 0;
  windows[user_list_win].x          = screen_width - screen_width / 8;
  windows[user_list_win].height     = screen_height - screen_height / 12 - screen_height / 3;
  windows[user_list_win].width      = screen_width / 8;
  windows[user_list_win].has_colors = colors_on;
  windows[user_list_win].has_text = false;
  windows[user_list_win].has_command_response = false;
  windows[user_list_win].command_response_duration = 0;
  windows[user_list_win].ptr        = NULL; 
  windows[user_list_win].text_index = -1;
  windows[user_list_win].text[0] = '\0';

  windows[timetable_list_win].y          = screen_height - screen_height / 12 - screen_height / 3;
  windows[timetable_list_win].x          = screen_width - screen_width / 8;
  windows[timetable_list_win].height     = screen_height / 3;
  windows[timetable_list_win].width      = screen_width / 8;
  windows[timetable_list_win].has_colors = colors_on;
  windows[timetable_list_win].has_text = false;
  windows[timetable_list_win].has_command_response = false;
  windows[timetable_list_win].command_response_duration = 0;
  windows[timetable_list_win].ptr        = NULL;
  windows[timetable_list_win].text_index = -1;
  windows[timetable_list_win].text[0] = '\0';
  
  windows[log_win].y          = 2 * screen_height / 3;
  windows[log_win].x          = 0;
  windows[log_win].height     = screen_height / 3 - screen_height / 12;
  windows[log_win].width      = 7 * screen_width / 8;
  windows[log_win].has_colors = colors_on;
  windows[log_win].has_text = false;
  windows[log_win].has_command_response = false;
  windows[log_win].command_response_duration = 0;
  windows[log_win].ptr        = NULL;
  windows[log_win].text_index = -1;
  windows[log_win].text[0] = '\0';
  
  windows[stats_win].y          = screen_height - screen_height / 12;
  windows[stats_win].x          = 7 * screen_width / 8;
  windows[stats_win].height     = screen_height / 24;
  windows[stats_win].width      = screen_width / 8;
  windows[stats_win].has_colors = colors_on;
  windows[stats_win].has_text = false;
  windows[stats_win].has_command_response = false;
  windows[stats_win].command_response_duration = 0;
  windows[stats_win].ptr        = NULL;
  windows[stats_win].text_index = -1;
  windows[stats_win].text[0] = '\0';

  windows[command_win].y          = screen_height - screen_height / 24;
  windows[command_win].x          = 0;
  windows[command_win].height     = screen_height / 24;
  windows[command_win].width      = screen_width;
  windows[command_win].has_colors = colors_on;
  windows[command_win].has_text = false;
  windows[command_win].has_command_response = false;
  windows[command_win].command_response_duration = COMMAND_DURATION;
  windows[command_win].ptr        = NULL;
  windows[command_win].text_index = -1;
  windows[command_win].text[0] = '\0';

  windows[help_win].y          = 0;
  windows[help_win].x          = 0;
  windows[help_win].height     = screen_height - screen_height / 24;
  windows[help_win].width      = screen_width;
  windows[help_win].has_colors = colors_on;
  windows[help_win].has_text = true;
  windows[help_win].has_command_response = false;
  windows[help_win].command_response_duration = 0;
  windows[help_win].ptr        = NULL;
  windows[help_win].text_index = -1;
  windows[help_win].text[0]       = '\0';
  
  windows[schedule_win].ptr = newwin(
    windows[schedule_win].height, windows[schedule_win].width, windows[schedule_win].y, windows[schedule_win].x
  );
  if (!windows[schedule_win].ptr) {
    print_error(nullptr, "windows[schedule_win].ptr == NULL");
    ret = nullptr;
    goto _return;
  }

  windows[user_list_win].ptr = newwin(
    windows[user_list_win].height, windows[user_list_win].width, windows[user_list_win].y, windows[user_list_win].x
  );
  if (!windows[user_list_win].ptr) {
    print_error(nullptr, "windows[user_list_win].ptr == NULL");
    ret = nullptr;
    goto _return;
  }

  windows[timetable_list_win].ptr = newwin(
    windows[timetable_list_win].height, windows[timetable_list_win].width, windows[timetable_list_win].y, windows[timetable_list_win].x
  );
  if (!windows[timetable_list_win].ptr) {
    print_error(nullptr, "windows[timetable_list_win].ptr == NULL");
    ret = nullptr;
    goto _return;
  }

  windows[log_win].ptr = newwin(windows[log_win].height, windows[log_win].width, windows[log_win].y, windows[log_win].x);
  if (!windows[log_win].ptr) {
    print_error(nullptr, "windows[log_win].ptr == NULL");
    ret = nullptr;
    goto _return;
  }

  windows[stats_win].ptr = newwin(windows[stats_win].height, windows[stats_win].width, windows[stats_win].y, windows[stats_win].x);
  if (!windows[stats_win].ptr) {
    print_error(nullptr, "windows[stats_win].ptr == null");
    ret = nullptr;
    goto _return;
  }

  windows[command_win].ptr = newwin(
    windows[command_win].height, windows[command_win].width, windows[command_win].y, windows[command_win].x
  );
  if (!windows[command_win].ptr) {
    print_error(nullptr, "windows[command_win].ptr == null");
    ret = nullptr;
    goto _return;
  }

  windows[help_win].ptr = newwin(
    windows[help_win].height, windows[help_win].width, windows[help_win].y, windows[help_win].x
  );
  if (!windows[help_win].ptr) {
    print_error(nullptr, "windows[help_win].ptr == null");
    ret = nullptr;
    goto _return;
  }

_return:
  return ret;
}

bool is_digit(char ch) {
  return (bool)(ch >= '0' && ch <= '9');
}

size_t len_uint(uint value) {
  size_t len = 1;
  for (; value; value /= 10, len++);
  return len;
}

bool is_valid_user(ID selected_user, ID *users, size_t s_users) {
  if (!users)
    return false;

  for (size_t i = 0; i < s_users; i++)
    if (users[i] == selected_user)
      return true;
  return false;
}

char peek(char *str, size_t index, size_t len_str) {
  if (str == NULL)
    return '\0';
  if (index >= len_str)
    return '\0';
  return str[index];
}

void consume(char *str, size_t *index, char ch, size_t len_str) {
  if (index == NULL)
    return;
  if (str == NULL)
    return;
  for (; *index < len_str && peek(str, *index, len_str) == ch; (*index)++);
}

void strcut(char *buffer, char *str, size_t start, size_t len_buffer, size_t len_str) {
  if (buffer == NULL)
    return;
  if (str == NULL)
    return;
  if (start + len_buffer > len_str)
    return;

  for (size_t i = 0; i < len_buffer; i++)
    buffer[i] = str[i + start];
  buffer[len_buffer] = '\0';
}

bool parse_word(char *str, const char *word, size_t len_str, const size_t len_word, size_t start) {
  if (str == NULL)
    return false;
  if (len_str < start + len_word)
    return false;
  if (str[start + len_word] != ' ' && str[start + len_word] != '\0')
    return false;

  char buffer[len_word + 1];
  strcut(buffer, str, start, len_word, len_str);
  return (bool)(strncmp(buffer, word, len_word) == 0);
}

bool get_command(Win *window) {
  int ch = wgetch(window->ptr);

  if (ch == ERR)
    return false;
  if (ch == '\n')
    return true;
  if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
    if (window->text_index > -1) {
      window->text[window->text_index--] = '\0';
      backspace_erase(window);
    }
    return false;
  }
  if (++window->text_index >= MAX_TEXT_SIZE) {
    window_erase_text(window);
    print_command_response(window, ERROR_COMMAND_SIZE);
    return false;
  }

  window->text[window->text_index] = ch;
  window->text[window->text_index + 1] = '\0';
  window->has_text = true;

  return false;
}

void window_erase_text(Win *window) {
  if (!window)
    return;

  window->text_index = -1;
  window->text[0] = '\0';
  window->has_text = false;
  werase(window->ptr);
}

void backspace_erase(Win *window) {
  if (!window)
    return;

  int y, x;
  getyx(window->ptr, y, x);

  if (x > 0) {
      wmove(window->ptr, y, x - 1);
      waddch(window->ptr, ' ');
      wmove(window->ptr, y, x - 1);
  }
}

int parse_command(
  Win *window, bool parse,
  FILE **file, bool *help_on,
  size_t *offset_timetable, size_t *offset_user,
  size_t *current_timetable, ID *current_user,
  TimeTable **timetables, ID **users,
  size_t *s_timetables, size_t *s_users
) {
  if (!window)
    return 0;
  if (!parse)
    return 0;

  size_t i = 0;
  if (peek(window->text, i, window->text_index + 1) != COMMAND)
    goto _command_invalid;
  if (window->text_index == 0) {
    window_erase_text(window);
    return 0;
  }

  switch (peek(window->text, ++i, window->text_index + 1)) {
    case HELP_SHORT: {
      if (window->text_index == 1 || parse_word(window->text, HELP, window->text_index + 1, HELP_LEN, i)) {
        if (!help_on)
          return 0;

        window_erase_text(window);
        *help_on = !*help_on;
        return 0;
      }
      break;
    }

    case QUIT_SHORT: {
      if (window->text_index == 1 || parse_word(window->text, QUIT, window->text_index + 1, QUIT_LEN, i)) {
        window_erase_text(window);
        return 1;
      }
      break;
    }

    case SET_SHORT: {
      bool set_short = peek(window->text, i + 1, window->text_index + 1) == ' ',
           parsed_set = parse_word(window->text, SET, window->text_index + 1, SET_LEN, i);
      if (!(set_short || parsed_set))
        break;

      if (parsed_set)
        i += SET_LEN;
      else
        i++;

      if (*help_on)
        return 0;

      consume(window->text, &i, ' ', window->text_index + 1);

      if (parse_word(window->text, SET_USER, window->text_index + 1, SET_USER_LEN, i)) {
        if (!current_user)
          return 0;
        if (!users)
          return 0;

        i += SET_USER_LEN;
        consume(window->text, &i, ' ', window->text_index + 1);

        char ch;
        ID selected_user = 0;
        for (; is_digit(ch = peek(window->text, i, window->text_index + 1)); i++)
          selected_user = 10 * selected_user + (ch - '0');

        if (ch != '\0') {
          if (ch != ' ')
            break;
          for (; i < window->text_index + 1; i++)
            if (window->text[i] != ' ')
              break;
        }
        
        window_erase_text(window);
        
        if (is_valid_user(selected_user, *users, *s_users))
          *current_user = selected_user;
        else
          print_command_response(window, ERROR_COMMAND_NO_USER_MATCHED);

      } else if (parse_word(window->text, SET_TIMETABLE, window->text_index + 1, SET_TIMETABLE_LEN, i)) {
        if (!current_timetable)
          return 0;
        if (!s_timetables)
          return 0;

        i += SET_TIMETABLE_LEN;
        consume(window->text, &i, ' ', window->text_index + 1);

        char ch;
        size_t selected_timetable = 0;
        for (; is_digit(ch = peek(window->text, i, window->text_index + 1)); i++)
          selected_timetable = 10 * selected_timetable + (ch - '0');

        if (ch != '\0') {
          if (ch != ' ')
            break;
          for (; i < window->text_index + 1; i++)
            if (window->text[i] != ' ')
              break;
        }

        window_erase_text(window);
        if (selected_timetable <= *s_timetables) {
          *current_timetable = selected_timetable;
          *s_users = get_users(&((*timetables)[*current_timetable]), users);
        } else
          print_command_response(window, ERROR_COMMAND_NO_USER_MATCHED);

      } else if (parse_word(window->text, SET_OFFSET, window->text_index + 1, SET_OFFSET_LEN, i)) {
        i += SET_OFFSET_LEN;
        consume(window->text, &i, ' ', window->text_index + 1);

        if (parse_word(window->text, SET_USER, window->text_index + 1, SET_USER_LEN, i)) {
          if (!offset_user)
            return 0;

          i += SET_USER_LEN;
          consume(window->text, &i, ' ', window->text_index + 1);

          char ch;
          size_t selected_offset = 0;
          for (; is_digit(ch = peek(window->text, i, window->text_index + 1)); i++)
            selected_offset = 10 * selected_offset + (ch - '0');

          if (ch != '\0') {
            if (ch != ' ')
              break;
            for (; i < window->text_index + 1; i++)
              if (window->text[i] != ' ')
                break;
          }

          *offset_user = selected_offset;
          window_erase_text(window);

        } else if (parse_word(window->text, SET_TIMETABLE, window->text_index + 1, SET_TIMETABLE_LEN, i)) {
          if (!offset_timetable)
            return 0;
             
          i += SET_TIMETABLE_LEN;
          consume(window->text, &i, ' ', window->text_index + 1);

          char ch;
          size_t selected_offset = 0;
          for (; is_digit(ch = peek(window->text, i, window->text_index + 1)); i++)
            selected_offset = 10 * selected_offset + (ch - '0');

          if (ch != '\0') {
            if (ch != ' ')
              break;
            for (; i < window->text_index + 1; i++)
              if (window->text[i] != ' ')
                break;
          }

          *offset_timetable = selected_offset;
          window_erase_text(window);

        } else {
          break;
        }

      } else {
        break;
      }
      
      return 0;
    }

    default:
      break;
  }
 
_command_invalid:
  window_erase_text(window);
  print_command_response(window, ERROR_COMMAND_INVALID);
  
  return 0;
}

void print_command_text(Win *window) {
  if (!window)
    return;
  if (!window->has_text)
    return;
  if (window->has_command_response)
    return;

  wmove(window->ptr, 0, 0);
  wprintw(window->ptr, "%s", window->text);
}

void print_command_response(Win *window, const char *msg) {
  if (!window)
    return;
  if (!msg)
    return;

  wmove(window->ptr, 0, 0);
  wprintw(window->ptr, "%s", msg);
  
  window->has_command_response = true;
  gettimeofday(&window->timer, NULL);
}

void handle_command_response(Win *window, struct timeval *current_time) {
  if (!window)
    return;
  if (!current_time)
    return;
  if (!window->has_command_response)
    return;

  double delta = (current_time->tv_sec - window->timer.tv_sec) + (current_time->tv_usec - window->timer.tv_usec)/ 1000000.0;
  if (delta < window->command_response_duration)
    return;

  werase(window->ptr);
  window->has_command_response = false;
  window->timer = *current_time;
}

Error display_timetables(Win *window, size_t selected_timetable, size_t s_timetables, size_t offset) {
  check_error(nullptr, window == NULL);

  if (offset >= s_timetables)
    offset = s_timetables - window->height - 1;

  size_t timetable_count_pos = (size_t)((double)(window->width - strlen(TT_LIT) - 2 - len_uint(s_timetables)) / 2);
  wmove(window->ptr, 1, timetable_count_pos);
  wprintw(window->ptr, "%s: %zu", TT_LIT, s_timetables);

  for (size_t i = 2; i < window->height - 1 && i + offset - 2 < s_timetables; i++) {
    size_t k = i + offset - 2;
    size_t timetable_id_pos = (window->width - len_uint(k)) / 2 - ((selected_timetable == k) ? strlen(SELECTED) : 0);
    wmove(window->ptr, i, timetable_id_pos);
    wprintw(window->ptr, "%s%zu", selected_timetable == k ? SELECTED : "", k);
  }

  return none;
}

Error display_users(Win *window, ID selected_user, ID *users, size_t s_users, size_t offset) {
  check_error(nullptr, window == NULL);
  check_error(nullptr, users == NULL);

  if (offset >= s_users)
    offset = s_users - window->height - 1;

  size_t user_count_pos = (size_t)((double)(window->width - strlen(USERS_LIT) - 2 - len_uint(s_users)) / 2);
  wmove(window->ptr, 1, user_count_pos);
  wprintw(window->ptr, "%s: %zu", USERS_LIT, s_users);

  for (size_t i = 2; i < window->height - 1 && i + offset - 2 < s_users; i++) {
    size_t k = i + offset - 2;
    size_t user_id_pos = (window->width - len_uint(users[k])) / 2 - ((selected_user == users[k]) ? strlen(SELECTED) : 0);
    wmove(window->ptr, i, user_id_pos);
    wprintw(window->ptr, "%s%u", selected_user == users[k] ? SELECTED : "", users[k]);
  }

  return none;
}

void draw_grid(WINDOW *win, int num_rows, int num_cols) {
    int height, width;
    getmaxyx(win, height, width);

    double row_height = (double)height / num_rows;
    double col_width = (double)width / num_cols;

    for (int i = 1; i < num_rows; i++)
      mvwhline(win, (size_t)(i * row_height), 1, '-', width - 2);

    for (int i = 1; i < num_cols; i++)
      mvwvline(win, 1, (size_t)(i * col_width), '|', height - 2);
}

Error display_user_schedule(Win *window, TimeTable *timetable, ID user) {
  check_error(nullptr, window == NULL);
  check_error(nullptr, timetable == NULL);

  draw_grid(window->ptr, timetable->n, timetable->m);

  Assignment *schedule = NULL;
  size_t s_schedule = type_get_assign_from_timetable_with_id(&schedule, timetable, user);
  check_error(nullptr, schedule == NULL);

  double height_table_entry = (double)window->height / timetable->n,
         width_table_entry = (double)window->width / timetable->m;

  for (size_t i = 0; i < s_schedule; i++) {
    size_t class_pos_y = height_table_entry / 2,
           class_pos_x = (double)width_table_entry / 2 - (double)len_uint(schedule[i].class) / 2,

           classroom_pos_y = height_table_entry / 2 + 1,
           classroom_pos_x = (double)width_table_entry / 2 - (double)len_uint(schedule[i].classroom) / 2,

           entry_pos_y = window->y + (size_t)(schedule[i].block.i * height_table_entry),
           entry_pos_x = window->x + (size_t)(schedule[i].block.j * width_table_entry);

    if (timetable->n <= 8) {
      mvwprintw(window->ptr, entry_pos_y + class_pos_y, entry_pos_x + class_pos_x, "%u", schedule[i].class);
      mvwprintw(window->ptr, entry_pos_y + classroom_pos_y, entry_pos_x + classroom_pos_x, "%u", schedule[i].classroom);
      continue;
    }

    size_t data_pos_y = class_pos_y,
           data_pos_x = width_table_entry / 2 - (len_uint(schedule[i].class) + len_uint(schedule[i].classroom)) / 2;
    mvwprintw(window->ptr, entry_pos_y + data_pos_y, entry_pos_x + data_pos_x, "%u/%u", schedule[i].class, schedule[i].classroom);
  }

  return none;
}

void update_fps(Win *fps_win, struct timeval *current_time, struct timeval *last_sec, uint *frame_counter) {
  if (!fps_win)
    return;
  if (!current_time)
    return;
  if (!last_sec)
    return;
  if (!frame_counter)
    return;

  double delta = (current_time->tv_sec - last_sec->tv_sec) + (current_time->tv_usec - last_sec->tv_usec) / 1000000.0;
  if (delta >= 1.000) {
    char fps_text[20];
    snprintf(fps_text, sizeof(fps_text), "%s%u", FPS_LIT, *frame_counter);
    wmove(fps_win->ptr, fps_win->height - 1, fps_win->width - (size_t)((double)strlen(fps_text) * 3/2));
    wprintw(fps_win->ptr, "%s", fps_text);

    *last_sec = *current_time;
    *frame_counter = 0;
  }
}

Error refresh_windows(
  Win windows[S_WINDOWS], bool help_on, size_t offset_timetables, size_t offset_user,
  struct timeval *current_time, struct timeval *last_refresh,
  uint *frame_counter, const double frame_time,
  size_t current_timetable, TimeTable *timetables, size_t s_timetables,
  ID current_user, ID *users, size_t s_users
) {
  check_error(nullptr, windows == NULL);
  check_error(nullptr, current_time == NULL);
  check_error(nullptr, last_refresh == NULL);
  check_error(nullptr, users == NULL);

  double elapsed_time = (current_time->tv_sec - last_refresh->tv_sec) + (current_time->tv_usec - last_refresh->tv_usec) / 1000000.0;
  if (elapsed_time >= frame_time) { 
    clear();

    if (!help_on) {
      werase(windows[schedule_win].ptr);
      werase(windows[user_list_win].ptr);
      werase(windows[timetable_list_win].ptr);
      // werase(windows[log_win].ptr);

      wbkgd(windows[schedule_win].ptr, COLOR_PAIR(1));
      wbkgd(windows[user_list_win].ptr, COLOR_PAIR(1));
      wbkgd(windows[timetable_list_win].ptr, COLOR_PAIR(1));
      // wbkgd(windows[log_win].ptr, COLOR_PAIR(1));
      wbkgd(windows[stats_win].ptr, COLOR_PAIR(1));
      
      box(windows[schedule_win].ptr, 0, 0);
      box(windows[user_list_win].ptr, 0, 0);
      box(windows[timetable_list_win].ptr, 0, 0);
      // box(windows[log_win].ptr, 0, 0);

      Error error;
      error = display_timetables(&windows[timetable_list_win], current_timetable, s_timetables, offset_timetables);
      check_error(error, error != none);
      error = display_users(&windows[user_list_win], current_user, users, s_users, offset_user);
      check_error(error, error != none);
      error = display_user_schedule(&windows[schedule_win], &timetables[current_timetable], current_user);
      check_error(error, error != none);

      wrefresh(windows[schedule_win].ptr);
      wrefresh(windows[user_list_win].ptr);
      wrefresh(windows[timetable_list_win].ptr);
      // wrefresh(windows[log_win].ptr);
      wrefresh(windows[stats_win].ptr);
    } else {
      werase(windows[help_win].ptr);
      wbkgd(windows[help_win].ptr, COLOR_PAIR(1));
      wmove(windows[help_win].ptr, 1, 1);
      wprintw(windows[help_win].ptr, "%s", help_text);
      wrefresh(windows[help_win].ptr);
    }
    
    wbkgd(windows[command_win].ptr, COLOR_PAIR(2));
    print_command_text(&windows[command_win]);
    handle_command_response(&windows[command_win], current_time);
    wrefresh(windows[command_win].ptr);

    *last_refresh = *current_time;
    (*frame_counter)++;
  }

  return none;
}
