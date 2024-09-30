#include "class_distribution.h"

// =====================================================# PUBLIC #=============================================================

void class_distribution(
  TimeTable *timetable, User *users, ID *classes,
  size_t s_users, size_t s_classes,
  const size_t max_classes, const size_t max_students_per_classroom
) {
  assert(nullptr, timetable != NULL);
  assert(nullptr, users != NULL);
  assert(nullptr, classes != NULL);
  assert(inval_arg, s_users != 0);
  assert(inval_arg, s_classes != 0);
  assert(inval_arg, max_classes != 0);
  assert(inval_arg, max_students_per_classroom != 0);

  HashMap *queue_map = hashmap_create(timetable->n * timetable->m);
  assert(nullptr, queue_map != NULL);

  struct EntryTable table = {
    .timetable   = timetable,
    .map         = queue_map,
    .max_classes = max_classes
  };

  size_t s_queue = timetable->n * timetable->m;
  Entry *queue = _class_distribution_create_queue(&table, users, s_users);
  assert(nullptr, queue != NULL);

  HashMap *class_map = _class_distribution_sort_classes(classes, s_classes, users, s_users);
  assert(nullptr, class_map != NULL);

  size_t min_value = _class_distribution_max_user_classes(users, s_users);
  assert(unexp_behaviour, min_value != 0);

  for (size_t i = 0; i < s_classes; i++) {
    size_t s_class_placements = _class_distribution_calculate_placements(
      classes[i], class_map, timetable->n, timetable->m, max_students_per_classroom, min_value
    );
    assert(unexp_behaviour, s_class_placements != 0);
    _class_distribution_sort_queue(queue, s_queue, &table);
    _class_distribution_place(&table, classes[i], s_class_placements, queue, s_queue);
  }

  hashmap_free(class_map, NULL);
  hashmap_free(queue_map, NULL);
  free(queue);
}

// =====================================================# PRIVATE #============================================================

void _class_distribution_place(EntryTable table, ID class, size_t s_placements, Entry *queue, size_t s_queue) {
  assert(nullptr, table != NULL);
  assert(nullptr, table->timetable != NULL);
  assert(nullptr, table->map != NULL);
  assert(nullptr, queue != NULL);
  assert(inval_arg, s_placements <= s_queue);
  assert(inval_arg, table->max_classes != 0);

  for (size_t i = 0; i < s_placements; i++) {
    Entry candidate = queue[i]; 
    TimeBlock *selected_timeblock = &table->timetable->entries[candidate.i][candidate.j];

    if (_class_distribution_in_timeblock(class, selected_timeblock->classrooms, selected_timeblock->s_classrooms))
      continue;

    for (size_t k = 0; k < selected_timeblock->s_classrooms; k++) {
      ClassRoom *classroom = &selected_timeblock->classrooms[k];
      if (classroom->s_classes >= table->max_classes)
        continue;

      size_t s_classes = classroom->s_classes;
      Class *classes = (Class *)realloc(classroom->classes, (s_classes + 1) * sizeof(Class));
      assert(mem_error, classes != NULL);

      classes[s_classes].id = class;
      classes[s_classes].s_students = 0;
      classes[s_classes].students = NULL;
      classes[s_classes].s_teachers = 0;
      classes[classroom->s_classes++].teachers = NULL;

      classroom->classes = classes;
      break;
    }
  }
}

size_t _class_distribution_calculate_placements(
  ID class, HashMap *class_map, const size_t n, const size_t m, const size_t max_students_per_classroom, const size_t min_value
) {
  assert(nullptr, class_map != NULL);
  assert(inval_arg, max_students_per_classroom != 0);

  char *key = _class_distribution_create_key(class);
  assert(nullptr, key != NULL);

  size_t *value = hashmap_get(class_map, key);
  assert(nullptr, value != NULL);

  free(key);

  size_t s_placements = (m * (*value)) / max_students_per_classroom + min_value;
  return s_placements > n * m ? n * m : s_placements;
}

size_t _class_distribution_max_user_classes(User *users, size_t s_users) {
  assert(nullptr, users != NULL);
  size_t max_value = type_get_size_classes(&users[0]);
  for (size_t i = 1; i < s_users; i++) {
    size_t s_classes = type_get_size_classes(&users[i]);
    if (max_value < s_classes)
      max_value = s_classes;
  }
  return max_value;
}

Entry *_class_distribution_create_queue(EntryTable table, User *users, size_t s_users) {
  assert(nullptr, table != NULL);
  assert(nullptr, table->timetable != NULL);
  assert(nullptr, table->map != NULL);
  assert(nullptr, users != NULL);
  assert(inval_arg, table->max_classes != 0);

  size_t n = table->timetable->n,
         m = table->timetable->m;

  Entry *queue = (Entry *)calloc(n * m, sizeof(Entry));
  assert(nullptr, queue != NULL);

  for (size_t i = 0; i < n; i++) 
    for (size_t j = 0; j < m; j++) {
      Entry entry = { i, j };
      queue[i * m + j] = entry;

      char *key = _class_distribution_create_key(i * 10 + j);
      assert(nullptr, key != NULL);

      int *score = (int *)malloc(sizeof(int));
      assert(nullptr, score != NULL);
      *score = _class_distribuition_user_score(&entry, users, s_users);

      hashmap_insert(table->map, key, (void *)score);

      free(key);
    }

  return queue;
}

uint _class_distribuition_user_score(Entry *entry, User *users, size_t s_users) {
  if (entry == NULL)
    return 0;
  if (users == NULL)
    return 0;

  uint score = 0;
  for (size_t i = 0; i < s_users; i++) {
    size_t s_avail = type_get_size_avail(&users[i]);
    Entry *avail = type_get_avail(&users[i]);
    for (size_t j = 0; j < s_avail; j++)
      if (avail[j].i == entry->i && avail[j].j == entry->j)
        score++;
  }

  return score;
}

HashMap *_class_distribution_sort_classes(ID *classes, size_t s_classes, User *users, size_t s_users) {
  assert(nullptr, classes != NULL);
  assert(nullptr, users != NULL);
  
  HashMap *class_map = hashmap_create(s_classes);
  assert(nullptr, class_map != NULL);
  
  for (size_t i = 0; i < s_classes; i++) {
    ID class = classes[i];

    size_t *value = (size_t *)malloc(sizeof(size_t));
    assert(nullptr, value != NULL);
    *value = 0;

    for (size_t j = 0; j < s_users; j++) {
      if (!type_get_is_student(&users[i]))
        continue;

      size_t s_user_classes = type_get_size_classes(&(users[i]));
      ID *user_classes = type_get_classes(&(users[i]));

      for (size_t k = 0; k < s_user_classes; k++)
        if (user_classes[k] == class) {
          (*value)++;
          break;
        }
    }

    char *key = _class_distribution_create_key(class);
    assert(nullptr, key != NULL);

    hashmap_insert(class_map, key, (void *)value);

    free(key);
  }

  _quicksort((void *)classes, 0, s_classes - 1, sizeof(ID), (void *)class_map, _class_gte_occurences);

  return class_map;
}

void _class_distribution_sort_queue(Entry *queue, size_t s_queue, EntryTable table) {
  _quicksort((void *)queue, 0, s_queue - 1, sizeof(Entry), (void *)table, _entrie_gte_score);
}

char *_class_distribution_create_key(uint value) {
  size_t s_str = _len_int(value) + 1;
  char *str = (char *)calloc(s_str, sizeof(char));
  if (str == NULL)
    return NULL;
  snprintf(str, s_str - 1, "%u", value);
  return str;
}

bool _class_distribution_in_timeblock(ID class, ClassRoom *classrooms, size_t s_classrooms) {
  if (!classrooms)
    return false;
  for (size_t i = 0; i < s_classrooms; i++) {
    size_t s_classes = classrooms[i].s_classes;
    Class *classes = classrooms[i].classes;
    if (!classes)
      continue;
    for (size_t j = 0; j < s_classes; j++)
      if (classes[j].id == class)
        return true;
  }
  return false;
}

bool _class_gte_occurences(void *valueA, void *valueB, void *context) {
  assert(nullptr, context != NULL);

  if (valueA == NULL)
    return false;
  if (valueB == NULL)
    return true;

  HashMap *map = (HashMap *)context;
  ID classA = *(ID *)(valueA),
     classB = *(ID *)(valueB);

  char *keyA = _class_distribution_create_key(classA),
       *keyB = _class_distribution_create_key(classB);

  if (keyA == NULL)
    return false;
  if (keyB == NULL)
    return false;

  size_t *occurencesA = (size_t *)hashmap_get(map, keyA),
         *occurencesB = (size_t *)hashmap_get(map, keyB);

  free(keyA);
  free(keyB);

  return occurencesA ? (occurencesB ? (bool)(*occurencesA >= *occurencesB) : true) : false;
}

bool _entrie_gte_score(void *valueA, void *valueB, void *context) {
  assert(nullptr, context != NULL);

  if (valueA == NULL)
    return false;
  if (valueB == NULL)
    return true;

  EntryTable table = (EntryTable)context;
  Entry entryA = *(Entry *)(valueA),
        entryB = *(Entry *)(valueB);

  TimeBlock timeblockA = table->timetable->entries[entryA.i][entryA.j],
            timeblockB = table->timetable->entries[entryB.i][entryB.j];

  size_t s_total_classes = 0;
  for (size_t i = 0; i < timeblockA.s_classrooms; i++)
    s_total_classes += timeblockA.classrooms[i].s_classes;
  if (s_total_classes > timeblockA.s_classrooms * table->max_classes)
    return false;

  s_total_classes = 0;
  for (size_t i = 0; i < timeblockB.s_classrooms; i++)
    s_total_classes += timeblockB.classrooms[i].s_classes;
  if (s_total_classes > timeblockB.s_classrooms * table->max_classes)
    return true;

  char *keyA = _class_distribution_create_key(entryA.i * 10 + entryA.j),
       *keyB = _class_distribution_create_key(entryB.i * 10 + entryB.j);

  if (keyA == NULL)
    return false;
  if (keyB == NULL)
    return false;

  int *scoreA = (int *)hashmap_get(table->map, keyA),
      *scoreB = (int *)hashmap_get(table->map, keyB);

  free(keyA);
  free(keyB);

  return scoreA ? (scoreB ? (bool)(*scoreA >= *scoreB) : true) : false;
}

int _partition(void *array, int low, int high, size_t type_size, void *context, bool sortby(void *valueA, void *valueB, void *context)) {
  if (array == NULL)
    return -1;

  char *arr = (char *)array;
  int pivot = high;
  int i = low - 1;

  void *temp = malloc(type_size);
  if (temp == NULL)
    return -1;

  for (int j = low; j < high; j++) {
    if (!sortby(arr + j * type_size, arr + pivot * type_size, context))
      continue;

    i++;
    memcpy(temp, arr + i * type_size, type_size);
    memcpy(arr + i * type_size, arr + j * type_size, type_size);
    memcpy(arr + j * type_size, temp, type_size);
  }
    
  memcpy(temp, arr + (i + 1) * type_size, type_size);
  memcpy(arr + (i + 1) * type_size, arr + pivot * type_size, type_size);
  memcpy(arr + pivot * type_size, temp, type_size);

  free(temp);
  return i + 1;
}

void _quicksort(void *array, int low, int high, size_t type_size, void *context, bool sortby(void *valueA, void *valueB, void *context)) {
  if (array == NULL)
    return;
  if (context == NULL)
    return;
  if (low >= high)
    return;

  int pivot = _partition(array, low, high, type_size, context, sortby);
  assert(unexp_behaviour, pivot != -1);

  _quicksort(array, low, pivot - 1, type_size, context, sortby);
  _quicksort(array, pivot + 1, high, type_size, context, sortby);
}

size_t _len_int(int value) {
  size_t len = 1;
  for (; value; value /= 10, len++);
  return len;
}
