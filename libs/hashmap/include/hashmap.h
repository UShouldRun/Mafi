#ifndef HASH_MAP
#define HASH_MAP

#include <stdlib.h>
#include <string.h>

#ifndef BOOL
#define BOOL

typedef enum bool {
  false = 0,
  true
} bool;

#endif // !BOOL

typedef struct HashMap_Pair {
  char *key;
  void *value;
  struct HashMap_Pair *next;
} HashMap_Pair;

typedef struct {
  size_t s_buckets;
  HashMap_Pair **buckets;
} HashMap;

// ==================================================# PUBLIC #=================================================================

HashMap *hashmap_create(size_t s_buckets);

size_t   hashmap_size(HashMap *hashmap);
bool     hashmap_exists(HashMap *hashmap, char *key);

void     hashmap_insert(HashMap *hashmap, char *key, void *value);
void    *hashmap_get(HashMap *hashmap, char *key);
void     hashmap_remove(HashMap *hashmap, char *key, void free_value(void *value));
void     hashmap_free(HashMap *hashmap, void free_value(void *value));

// ==================================================# PRIVATE #=================================================================

size_t   hash(char *key, size_t s_buckets);

#endif
