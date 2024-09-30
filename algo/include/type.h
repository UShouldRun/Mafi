#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned ID;

#ifndef BOOL
#define BOOL

typedef enum bool {
  false = 0,
  true
} bool;

#endif // !BOOL

typedef enum {
  student_type = 1,
  teacher_type
} UserType;

typedef struct {
  size_t i, j;
} Entry;

typedef struct {
  Entry block;
  ID classroom,
     class;
} Assignment;

typedef struct {
  ID id;
  size_t s_students,
         s_teachers;
  ID *students,
     *teachers;
} Class;

typedef struct{
  ID id;
  size_t s_classes;
  Class *classes;
} ClassRoom;

typedef struct {
  Entry entry;
  size_t s_classrooms;
  ClassRoom *classrooms;
} TimeBlock;

typedef struct {
  size_t n, m;
  TimeBlock **entries;
} TimeTable;

typedef struct {
  size_t s_classes,
         s_avail,
         s_assign;
  ID id,
     *classes;
  Entry *avail;
} Student;

typedef struct {
  size_t s_classes,
         s_avail,
         s_assign;
  ID id,
     *classes;
  Entry *avail;
} Teacher;

typedef struct {
  UserType type;
  union {
    Student student;
    Teacher teacher;
  } select;  
} User;

Class *type_create_class(ID id, ID *students, ID *teachers, size_t s_students, size_t s_teachers);
ClassRoom *type_create_classroom(ID id, Class *classes, size_t s_classes);
TimeBlock *type_create_timeblock(Entry *entry, ClassRoom *classrooms, size_t s_classrooms);
TimeTable *type_create_timetable(TimeBlock **entries, size_t n, size_t m);
Student *type_create_student(ID id, ID *classes, Entry *avail, size_t s_classes, size_t s_avail);
Teacher *type_create_teacher(ID id, ID *classes, Entry *avail, size_t s_classes, size_t s_avail);
User *type_create_user(UserType type, ID id, ID *classes, Entry *avail, size_t s_classes, size_t s_avail);

ID *type_copy_user_ids(ID *users, size_t s_users);
Class *type_copy_classes(Class *classes, size_t s_classes);
ClassRoom *type_copy_classrooms(ClassRoom *classrooms, size_t s_classrooms);
TimeBlock *type_copy_timeblocks(TimeBlock *timeblocks, size_t s_timeblocks);
TimeTable *type_copy_timetable(TimeTable *timetable);

bool type_get_is_student(User *user);
size_t type_get_size_classes(User *user);
size_t type_get_size_avail(User *user);
size_t type_get_size_assign(User *user);
size_t type_get_classroom_current_capacity(ClassRoom *classroom);
ID type_get_id(User *user);
ID *type_get_classes(User *user);
Entry *type_get_avail(User *user);

size_t type_get_assign_from_timetable(Assignment **assign, TimeTable *timetable, User *user);
size_t type_get_assign_from_timetable_with_id(Assignment **assign, TimeTable *timetable, ID user);

void type_set_size_classes(User *user, size_t s_users);
void type_set_size_avail(User *user, size_t s_avail);
void type_set_size_assign(User *user, size_t s_assign);
void type_set_id(User *user, ID id);
void type_set_classes(User *user, ID *classes);
void type_set_avail(User *user, Entry *avail);

void type_delete_class(Class **class, size_t s_classes);
void type_delete_classroom(ClassRoom **classroom, size_t s_classrooms);
void type_delete_timeblock(TimeBlock **timeblock, size_t s_timeblocks);
void type_delete_timetable(TimeTable **timetable);
void type_delete_student(Student **student);
void type_delete_teacher(Teacher **teacher);
void type_delete_user(User **user);

void type_print_timetable(FILE *dest, TimeTable *timetable);
void type_print_user(FILE *dest, User *user);

#endif // TYPE_H
