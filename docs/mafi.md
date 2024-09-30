# MAFI: Scheduling Management API

**MAFI** is a scheduling API written in C, designed to handle scheduling and timetable generation for a given set of users, classrooms, and classes. It uses an A* algorithm to find near-optimal solutions to placement problems. The API supports file-based input with strict grammar rules and a CLI tool for visualizing the output.

## Table of Contents
1. [Introduction](#introduction)
2. [API Overview](#api-overview)
   - [Type](#type)
   - [Class Distribution](#class-distribution)
   - [Algo](#algo)
   - [Utils](#utils)
   - [Core](#core-functions)
   - [Command-Line Interface (CLI)](#command-line-interface)
3. [License](#license)

---

## Introduction

Welcome to the **MAFI API** documentation! This guide will help you understand how to use MAFI to manage scheduling problems, as well as the accompanying CLI tool for viewing the results.

> **Note**: MAFI accepts input files with strict grammar (`mr`), and these rules are defined in the `/docs` directory. Please refer to these files for formatting requirements.

## API Overview

This project is a combination of various different subsystems, but the main library `algo` will be the most important for you to worry about.

`algo` has 3 main components to it:
- `type`: types declaration and useful data processing functions
- `class_distribution`: a single public easy to use function with a relatively simple and easy to understand system behind it
- `algo`: implementation of the main functions used in the runtime of **Mafi**

Besides `algo` the project main file is inside the `src` directory (`src/main.c`).

`src` also has 2 important components:
- `core`: where you'll find the implementation of the algo functions in the `mafi` function as well as the `read` and `store` functions
- `utils`: extra functions and macros to complement the behaviour of the overall system

---

### Type

---

- Structs declaration in `algo/include/type.h`.

---

#### Creation Functions

- `Class *type_create_class(ID id, ID *students, ID *teachers, size_t s_students, size_t s_teachers);`
  - **Description**: Returns a pointer to a dynamically allocated `Class` struct, given an ID, an array of student IDs, and an array of teacher IDs.

- `ClassRoom *type_create_classroom(ID id, Class *classes, size_t s_classes);`
  - **Description**: Returns a pointer to a dynamically allocated `ClassRoom` struct, given an ID and an array of classes.

- `TimeBlock *type_create_timeblock(Entry *entry, ClassRoom *classrooms, size_t s_classrooms);`
  - **Description**: Returns a pointer to a dynamically allocated `TimeBlock` struct, given an entry and an array of classrooms.

- `TimeTable *type_create_timetable(TimeBlock **entries, size_t n, size_t m);`
  - **Description**: Returns a pointer to a dynamically allocated `TimeTable` struct, given an array of entries and the size of the matrix.

- `Student *type_create_student(ID id, ID *classes, Entry *avail, size_t s_classes, size_t s_avail);`
  - **Description**: Returns a pointer to a dynamically allocated `Student` struct, given an ID, an array of class IDs, and an availability array.

- `Teacher *type_create_teacher(ID id, ID *classes, Entry *avail, size_t s_classes, size_t s_avail);`
  - **Description**: Returns a pointer to a dynamically allocated `Teacher` struct, given an ID, an array of class IDs, and an availability array.

- `User *type_create_user(UserType type, ID id, ID *classes, Entry *avail, size_t s_classes, size_t s_avail);`
  - **Description**: Returns a pointer to a dynamically allocated `User` struct, given a user type, ID, an array of class IDs, and an availability array.

---

#### Copy Functions

- `ID *type_copy_user_ids(ID *users, size_t s_users);`
  - **Description**: Returns a pointer to a dynamically allocated copy of the input array of user IDs.

- `Class *type_copy_classes(Class *classes, size_t s_classes);`
  - **Description**: Returns a pointer to a dynamically allocated copy of the input array of classes.

- `ClassRoom *type_copy_classrooms(ClassRoom *classrooms, size_t s_classrooms);`
  - **Description**: Returns a pointer to a dynamically allocated copy of the input array of classrooms.

- `TimeBlock *type_copy_timeblocks(TimeBlock *timeblocks, size_t s_timeblocks);`
  - **Description**: Returns a pointer to a dynamically allocated copy of the input array of time blocks.

- `TimeTable *type_copy_timetable(TimeTable *timetable);`
  - **Description**: Returns a pointer to a dynamically allocated copy of the input timetable.

---

#### Get Functions

- `bool type_get_is_student(User *user);`
  - **Description**: Returns `true` if the user is a student, and `false` otherwise.

- `size_t type_get_size_classes(User *user);`
  - **Description**: Returns the size of the user's class array.

- `size_t type_get_size_avail(User *user);`
  - **Description**: Returns the size of the user's availability array.

- `size_t type_get_size_assign(User *user);`
  - **Description**: Returns the number of times the user has been assigned (how many times the algorithm has attempted to assign the user to time blocks).

- `size_t type_get_classroom_current_capacity(ClassRoom *classroom);`
  - **Description**: Returns the current total number of students assigned to the classroom.

- `ID type_get_id(User *user);`
  - **Description**: Returns the ID of the user.

- `ID *type_get_classes(User *user);`
  - **Description**: Returns a pointer to the user's array of class IDs.

- `Entry *type_get_avail(User *user);`
  - **Description**: Returns a pointer to the user's availability array.

- `size_t type_get_assign_from_timetable(Assignment **assign, TimeTable *timetable, User *user);`
  - **Description**: Returns the number of times a user is assigned to different time blocks across the timetable. Modifies the `assign` pointer to include an array of `Assignment` structs if `assign != NULL`.

- `size_t type_get_assign_from_timetable_with_id(Assignment **assign, TimeTable *timetable, ID user);`
  - **Description**: Returns the number of times a user ID is assigned to different time blocks across the timetable. Modifies the `assign` pointer to include an array of `Assignment` structs if `assign != NULL`. This is usually a better option than the previous function.

---

#### Set Functions

- `void type_set_size_classes(User *user, size_t s_classes);`
  - **Description**: Sets the size of the user's class array to `s_classes`.

- `void type_set_size_avail(User *user, size_t s_avail);`
  - **Description**: Sets the size of the user's availability array to `s_avail`.

- `void type_set_size_assign(User *user, size_t s_assign);`
  - **Description**: Sets the number of times the user has been assigned to `s_assign`.

- `void type_set_id(User *user, ID id);`
  - **Description**: Sets the user's ID to `id`.

- `void type_set_classes(User *user, ID *classes);`
  - **Description**: Sets the user's classes pointer to `classes`.

- `void type_set_avail(User *user, Entry *avail);`
  - **Description**: Sets the user's availability pointer to `avail`.

---

#### Delete Functions

- `void type_delete_class(Class **class, size_t s_classes);`
  - **Description**: Frees the memory associated with the `Class` array.

- `void type_delete_classroom(ClassRoom **classroom, size_t s_classrooms);`
  - **Description**: Frees the memory associated with the `ClassRoom` array.

- `void type_delete_timeblock(TimeBlock **timeblock, size_t s_timeblocks);`
  - **Description**: Frees the memory associated with the `TimeBlock` array.

- `void type_delete_timetable(TimeTable **timetable);`
  - **Description**: Frees the memory associated with the `TimeTable` object.

- `void type_delete_student(Student **student);`
  - **Description**: Frees the memory associated with the `Student` object.

- `void type_delete_teacher(Teacher **teacher);`
  - **Description**: Frees the memory associated with the `Teacher` object.

- `void type_delete_user(User **user);`
  - **Description**: Frees the memory associated with the `User` object.

---

#### Print Functions

- `void type_print_timetable(FILE *dest, TimeTable *timetable);`
  - **Description**: Prints the timetable to the specified file.

- `void type_print_user(FILE *dest, User *user);`
  - **Description**: Prints the user information to the specified file.

---

### Class Distribution

---

- Structs declaration in `algo/include/class_distribution.h`

---

#### Class Distribution Public Functions

- `void class_distribution(TimeTable *timetable, User *users, ID *classes, size_t s_users, size_t s_classes, const size_t max_classes, const size_t max_students_per_classroom);`
  - **Description**: Distributes classes across the timetable based on user availability and classroom constraints.

---

#### Class Distribution Private Functions

- `void _class_distribution_place(EntryTable table, ID class, size_t s_placements, Entry *queue, size_t s_queue);`
  - **Description**: Places a class in a timeblock based on a given queue and the number of placements.

- `size_t _class_distribution_calculate_placements(ID class, HashMap *class_map, const size_t n, const size_t m, const size_t max_students_per_classroom, const size_t min_value);`
  - **Description**: Calculates how many placements are required for a class based on the size of the timetable, class map, and classroom constraints.

- `size_t _class_distribution_max_user_classes(User *users, size_t s_users);`
  - **Description**: Returns the maximum number of classes any user in the array has.

- `Entry *_class_distribution_create_queue(EntryTable table, User *users, size_t s_users);`
  - **Description**: Creates and returns a queue (array of `Entry` structs) based on the users' availability for distributing classes.

- `uint _class_distribuition_user_score(Entry *entry, User *users, size_t s_users);`
  - **Description**: Calculates and returns the score for an entry based on the availability of users for that timeblock.

- `HashMap *_class_distribution_sort_classes(ID *classes, size_t s_classes, User *users, size_t s_users);`
  - **Description**: Sorts the classes based on user availability and returns a map of class IDs to their occurrence counts.

- `void _class_distribution_sort_queue(Entry *queue, size_t s_queue, EntryTable table);`
  - **Description**: Sorts the queue based on user scores for the available timeblocks.

- `char *_class_distribution_create_key(uint value);`
  - **Description**: Creates a key string for the given integer value, used for hashmap operations.

- `bool _class_distribution_in_timeblock(ID class, ClassRoom *classrooms, size_t s_classrooms);`
  - **Description**: Checks if a class is already assigned to a timeblock in the provided classrooms.

- `bool _class_gte_occurences(void *valueA, void *valueB, void *context);`
  - **Description**: Compares two occurrence values to determine if the first is greater than or equal to the second.

- `bool _entrie_gte_score(void *valueA, void *valueB, void *context);`
  - **Description**: Compares two entries based on their scores.

- `int _partition(void *array, int low, int high, size_t type_size, void *context, bool sortby(void *valueA, void *valueB, void *context));`
  - **Description**: Partitions the array for quicksort based on a provided comparison function.

- `void _quicksort(void *array, int low, int high, size_t type_size, void *context, bool sortby(void *valueA, void *valueB, void *context));`
  - **Description**: Sorts the given array using the quicksort algorithm and a comparison function.

- `size_t _len_int(int value);`
  - **Description**: Returns the length of an integer when converted to a string representation.

---

### Algo

---

#### Algo Public Functions

- `void algo_free_state(void *state);`
  - **Description**: Frees the memory associated with a given state.

- `void algo_queue(Logger *logger, Exception *exception, User ***users, size_t *size);`
  - **Description**: Adds users to the queue and manages logging and exceptions.

- `void algo_pick(Logger *logger, Exception *exception, User **user, User **users, size_t size);`
  - **Description**: Picks a user from the provided array of users, handling logging and exceptions.

- `void algo_a_star_best(Logger *logger, Exception *exception, Tree *root, Tree **node, void *context, bool eval(void *stateA, void *stateB, void *context));`
  - **Description**: Implements the A* algorithm to find the best path in a search tree.

- `void algo_search(Logger *logger, Exception *exception, User *user, TimeTable *timetable, Assignment **schedule, size_t *size, size_t max_classroom_size, void *rules, bool in_rules(void *rules, TimeTable *timetable, Entry *block, ID class, ID user_id, bool is_student), size_t get_classrooms(ID **available_classrooms, TimeTable *timetable, Entry *block, ID class, size_t max_classroom_size));`
  - **Description**: Searches for a schedule for a user in a timetable based on various constraints and rules.

- `void algo_update_tree(Logger *logger, Exception *exception, Tree **leaf, User *user, Assignment *schedule);`
  - **Description**: Updates the search tree with the latest user and schedule information.

- `void algo_update_timetables(Logger *logger, Exception *exception, Tree *root, TimeTable ***possible_states, User **users, size_t *s_possible_states, size_t s_users);`
  - **Description**: Updates possible timetable states based on the current search tree and users.

---

#### Algo Private Functions

- `bool _algo_smaller_avail(User *userA, User *userB);`
  - **Description**: Compares the availability of two users to determine which one is smaller.

- `bool _algo_is_final_state(TimeTable *state, User **users, size_t size);`
  - **Description**: Checks if the given state represents a final solution based on user availability.

- `void _algo_check_null_users(Logger *logger, Exception *exception, User **users, size_t size);`
  - **Description**: Checks for null users in the array and logs any issues.

- `void _algo_cut_null_users(Logger *logger, Exception *exception, User ***users, size_t *size);`
  - **Description**: Removes null users from the user array, updating the size accordingly.

- `void _algo_partition(Logger *logger, Exception *exception, User **users, int low, int high, int *pivot, bool sortby(User *userA, User *userB));`
  - **Description**: Partitions the user array for quicksort based on a comparison function.

- `void _algo_recursion(Logger *logger, Exception *exception, User **users, int low, int high, bool sortby(User *userA, User *userB));`
  - **Description**: Performs recursive quicksort on the user array.

- `void _algo_quicksort(Logger *logger, Exception *exception, User **users, size_t size, bool sortby(User *userA, User *userB));`
  - **Description**: Sorts the user array using the quicksort algorithm and a comparison function.

---

### Utils

---

#### Exception Handling Macros

- `#define invalid_format(file_name)`
  - **Description**: Handles invalid format errors by logging the error and jumping to the return label.
  
- `#define raise_stop(condition)`
  - **Description**: Stops execution and logs a message if the specified condition is true.

- `#define raise_exception(condition, level, type)`
  - **Description**: Raises an exception and logs it if the specified condition is true. The level determines the severity.

- `#define raise_ptr_exception(condition)`
  - **Description**: Raises a null pointer exception and logs it if the specified condition is true.

- `#define handle_exception(exception)`
  - **Description**: Checks and handles exceptions using the provided logger and report.

---

#### Logging Macros

- `#define log_custom_status_message(format, param)`
  - **Description**: Logs a custom status message using the specified format and parameter.

- `#define log_custom_exception_msg(condition, format, param, level, type)`
  - **Description**: Logs a custom exception message if the specified condition is true.

- `#define run_log_custom_exception_msg(condition, format, param, level, type)`
  - **Description**: Logs a custom exception message and returns a type if the specified condition is true.

- `#define run_raise_exception(condition, level, type)`
  - **Description**: Raises an exception and returns a type if the specified condition is true.

---

### Function Declarations

- `void check_snprintf(FILE *logger_file, int number_characters, size_t s_buffer);`
  - **Description**: Checks the result of `snprintf` and logs any potential issues.

- `bool valid_classes_in_all_classrooms(TimeTable *timetable, size_t max_classes);`
  - **Description**: Validates that all classrooms in the timetable adhere to the maximum class constraints.

- `bool compare_timetable(void *stateA, void *stateB, void *context);`
  - **Description**: Compares two timetables to determine if they are equivalent.

- `bool less_than_max_students_per_classroom(size_t max_students_per_classroom, TimeBlock *timeblock, ID class);`
  - **Description**: Checks if the number of students in a timeblock is less than the specified maximum.

- `bool less_than_max_same_class_per_week_per_student(size_t max_same_class_per_week_per_student, TimeTable *timetable, ID class, ID id);`
  - **Description**: Validates if a student has less than the maximum number of same classes per week.

- `bool less_than_max_blocks_per_day_per_student(size_t max_blocks_per_day_per_student, TimeTable *timetable, size_t j, ID id);`
  - **Description**: Checks if a student has less than the maximum number of blocks assigned per day.

- `bool morning_and_afternoon(TimeTable *timetable, Entry *entry, ID id);`
  - **Description**: Determines if a student is scheduled for both morning and afternoon classes.

- `bool already_in_class(TimeBlock *timeblock, ID id);`
  - **Description**: Checks if a student is already assigned to a class in the specified timeblock.

- `bool in_rules(void *rules, TimeTable *timetable, Entry *entry, ID class, ID user_id, bool is_student);`
  - **Description**: Checks if a class assignment complies with the given rules for the user.
  
- `size_t get_classrooms(ID **available_classrooms, TimeTable *timetable, Entry *block, ID class, size_t max_size);`
  - **Description**: Retrieves available classrooms for a specified class within the given time block.

- `void _invalid_format(Logger *logger, Report *report, char *file_name, const char *file, const char *func, uint line);`
  - **Description**: Logs an invalid format error for a given file, including details about the location in the code.

---

### Core

- `void read(Logger *logger, Report *report, char *read_file_name, size_t *depth, Rules *rules, TimeTable **root_state, ID **classrooms, ID **classes, User ***users, size_t *s_classrooms, size_t *s_classes, size_t *s_users);`
  - **Description**: Reads data from a specified input file and populates the provided structures, including classrooms, classes, and users.

- `void store(Logger *logger, Report *report, char *write_file_name, TimeTable **timetables, size_t s_timetables);`
  - **Description**: Stores the current timetables in the specified output file, logging any issues encountered.

- `void mafi(Logger *logger, Report *report, double acceptance_rate, User ***users, TimeTable ***possible_timetables, size_t *s_users, size_t *s_possible_timetables, size_t depth, TimeTable *root_state, Rules *rules);`
  - **Description**: Executes the MAFI algorithm, utilizing the specified parameters to generate possible timetables based on user input and rules.

---

### Command-Line Interface (CLI)

```bash
mafi <acceptance_rate: ]0,1[> <input_file>.mr <output_file>.mw <log_file>.log
```

Replace `input_file.mr` with your input file (`.mr` format), `output_file.mw` (`.mw` format), where you'd like the output file to be written to, and the acceptance rate as the region of almost exact solutions for the program to find. If desired, you can add the path to a file where the program logs will be written to. If nothing is added it will directly print them to the `stdout`.

An example usage of this command would be:

```bash
mafi 0.90 path/to/myfile.mr path/to/myfile.mw path/to/myfile.log
```

---

## License

 Copyright (c) 2024 Henrique Teixeira
 
 This file is part of Mafi, licensed under the MIT License.
 
 See the LICENSE file in the project root for full license information.
