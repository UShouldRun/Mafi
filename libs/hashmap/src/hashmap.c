#include "hashmap.h"

// ==================================================# PUBLIC #=================================================================

HashMap *hashmap_create(size_t s_buckets) {
  if (s_buckets == 0)
    return NULL;

  HashMap *hashmap = (HashMap *)malloc(sizeof(HashMap));
  if (hashmap == NULL)
    return NULL;

  hashmap->buckets = (HashMap_Pair **)calloc(s_buckets, sizeof(HashMap_Pair *));
  if (hashmap->buckets == NULL)
    return NULL;

  hashmap->s_buckets = s_buckets;
  return hashmap;
}

size_t hashmap_size(HashMap *hashmap) {
  if (hashmap == NULL)
    return 0;

  size_t s_elements = 0;
  for (size_t i = 0; i < hashmap->s_buckets; i++)
    for (HashMap_Pair *current = hashmap->buckets[i]; current != NULL; current = current->next, s_elements++); 
  return s_elements;
}

bool hashmap_exists(HashMap *hashmap, char *key) {
  if (hashmap == NULL)
    return false;
  if (key == NULL)
    return false;

  size_t index = hash(key, hashmap->s_buckets);
  HashMap_Pair *current = hashmap->buckets[index];
  for (; current != NULL; current = current->next)
    if (strcmp(current->key, key) == 0)
      return true;
  return false;
}

void hashmap_insert(HashMap *hashmap, char *key, void *value) {
  if (hashmap == NULL)
    return;
  if (key == NULL)
    return;
  if (value == NULL)
    return;
 
  size_t index = hash(key, hashmap->s_buckets);
  HashMap_Pair **current = &(hashmap->buckets[index]);
  for (; *current != NULL; current = &((*current)->next)) {
    if (strcmp((*current)->key, key) != 0)
      continue;
    (*current)->value = value;
    return;
  }

  HashMap_Pair *new_pair = (HashMap_Pair *)malloc(sizeof(HashMap_Pair));
  if (new_pair == NULL)
    return;

  new_pair->key = strdup(key);
  if (new_pair->key == NULL) {
    free(new_pair);
    return;
  }

  new_pair->value = value;
  new_pair->next = NULL;

  *current = new_pair;
}

void *hashmap_get(HashMap *hashmap, char *key) {
  if (hashmap == NULL)
    return NULL;
  if (key == NULL)
    return NULL;

  size_t index = hash(key, hashmap->s_buckets);
  HashMap_Pair *current = hashmap->buckets[index];
  for (; current != NULL; current = current->next)
    if (strcmp(current->key, key) == 0)
      return current->value;
  return NULL;
}

void hashmap_remove(HashMap *hashmap, char *key, void free_value(void *value)) {
  if (hashmap == NULL)
    return;
  if (key == NULL)
    return;

  size_t index = hash(key, hashmap->s_buckets);
  HashMap_Pair **current = &(hashmap->buckets[index]);

  for (; *current != NULL; current = &((*current)->next)) {
    if (strcmp((*current)->key, key) != 0)
      continue;

    HashMap_Pair *pair_to_remove = *current;
    *current = (*current)->next;

    if (free_value) {
      free_value(pair_to_remove->value);
    } else {
      free(pair_to_remove->value);
    }
    
    free(pair_to_remove->key);
    free(pair_to_remove);
    return;
  }
}

void hashmap_free(HashMap *hashmap, void free_value(void *value)) {
  if (hashmap == NULL)
    return;

  for (size_t i = 0; i < hashmap->s_buckets; i++) {
    HashMap_Pair *current = hashmap->buckets[i];
    while (current != NULL) {
      HashMap_Pair *pair_to_free = current;
      current = current->next;

      if (free_value) {
        free_value(pair_to_free->value);
      } else {
        free(pair_to_free->value);
      }

      free(pair_to_free->key);
      free(pair_to_free);
    }
  }
  free(hashmap->buckets);
  free(hashmap);
}

// ==================================================# PRIVATE #=================================================================

size_t hash(char *key, size_t s_buckets) {
  if (key == NULL)
    return 0;
  if (s_buckets == 0)
    return 0;

  size_t len = strlen(key);
  size_t hash_value = 0;
  for (size_t i = 0; i < len; i++)
    hash_value = hash_value * 31 + key[i];

  if (len >= 2) {
    short a = (key[0] + key[1]) << 16,
          b = key[len - 2] + key[len - 1];
    hash_value ^= (a | b);
  }

  return hash_value % s_buckets;
}
