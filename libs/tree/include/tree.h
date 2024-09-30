#ifndef TREE_H
#define TREE_H

#include <stdlib.h>

#ifndef BOOL
#define BOOL

typedef enum {
  false = 0,
  true
} bool;

#endif // !BOOL

typedef struct tree_node {
  void *state;
  struct tree_node *parent;
  struct tree_node **children;
  size_t s_children;
} Tree;

// ======================================# PUBLIC #=========================================

Tree *tree_library_create_root(void *state);

Tree *tree_library_add_child(Tree *node, void *state);
size_t tree_library_get_size_children(Tree *node);

bool tree_library_is_leaf(Tree *node);
Tree **tree_library_get_leaves(Tree *root, size_t *size);

void tree_library_delete_node(Tree **root, Tree *node, bool equal(Tree *nodeA, Tree *nodeB), void free_state(void *state));
void tree_library_delete_tree(Tree **root, void free_state(void *state));

// ======================================# PRIVATE #=========================================

Tree **_tree_recursion(Tree *node, Tree **leaves, size_t *size);

#endif //TREE_H
