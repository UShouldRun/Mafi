#ifndef CLASS_DISTRIBUTION
#define CLASS_DISTRIBUTION

#include "error.h"
#include "type.h"
#include "hashmap.h"

// =====================================================# PUBLIC #=============================================================

void class_distribution(
  TimeTable *timetable, User *users, ID *classes,
  size_t s_users, size_t s_classes,
  const size_t max_classes, const size_t max_students_per_classroom
);

// =====================================================# PRIVATE #============================================================

typedef struct EntryTable {
  TimeTable *timetable;
  HashMap *map;
  size_t max_classes;
} *EntryTable;

void     _class_distribution_place(EntryTable table, ID class, size_t s_placements, Entry *queue, size_t s_queue);
size_t   _class_distribution_calculate_placements(
  ID class, HashMap *class_map, const size_t n, const size_t m, const size_t max_students_per_classroom, const size_t min_value
);
size_t   _class_distribution_max_user_classes(User *users, size_t s_users);

Entry   *_class_distribution_create_queue(EntryTable table, User *users, size_t s_users);
uint     _class_distribuition_user_score(Entry *entry, User *users, size_t s_users);

HashMap *_class_distribution_sort_classes(ID *classes, size_t s_classes, User *users, size_t s_users);
void     _class_distribution_sort_queue(Entry *queue, size_t s_queue, EntryTable table);

char    *_class_distribution_create_key(uint value);

bool     _class_distribution_in_timeblock(ID class, ClassRoom *classrooms, size_t s_classrooms);
bool     _class_gte_occurences(void *valueA, void *valueB, void *context);
bool     _entrie_gte_score(void *valueA, void *valueB, void *context);
int      _partition(void *array, int low, int high, size_t type_size, void *context, bool sortby(void *valueA, void *valueB, void *context));
void     _quicksort(void *array, int low, int high, size_t type_size, void *context, bool sortby(void *valueA, void *valueB, void *context));

size_t   _len_int(int value);

#endif
