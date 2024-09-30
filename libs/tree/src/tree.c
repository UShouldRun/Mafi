#include "tree.h"

// ======================================# PUBLIC #=========================================

Tree *tree_library_create_root(void *state) {
  if (!state)
    return NULL;

  Tree *root = (Tree *)malloc(sizeof(Tree));
  if (!root)
    return NULL;
  
  root->state = state;
  root->parent = NULL;
  root->children = NULL;
  root->s_children = 0;

  return root;
}

Tree *tree_library_add_child(Tree *node, void *state) {
  if (!node)
    return NULL;
  if (!state)
    return NULL;

  Tree *child = (Tree *)malloc(sizeof(Tree));
  if (!child)
    return NULL;

  child->state = state;
  child->parent = node;
  child->children = NULL;
  child->s_children = 0;

  Tree **temp = (Tree **)realloc(node->children, (node->s_children+1)*sizeof(Tree *));
  if (!temp)
    return NULL;
  node->children = temp;
  node->children[node->s_children++] = child;

  return node;
}

size_t tree_library_get_size_children(Tree *node) {
  return node ? node->s_children : 0;
}

bool tree_library_is_leaf(Tree *node) {
  return node ? (bool)(!node->children) : false;
}

Tree **tree_library_get_leaves(Tree *root, size_t *size) {
  *size = 0;
  return _tree_recursion(root, NULL, size);
}

void tree_library_delete_node(Tree **root, Tree *node, bool equal(Tree *nodeA, Tree *nodeB), void free_state(void *state)) {
  if (!root)
    return;
  if (!node)
    return;

  if (equal(*root, node)) {
    tree_library_delete_tree(root, free_state);
    return;
  }

  for (size_t i = 0; i < (*root)->s_children; i++) {
    tree_library_delete_node(&(*root)->children[i], node, equal, free_state);

    if (!(*root)->children[i]) {
      for (size_t j = i; j < (*root)->s_children - 1; j++)
        (*root)->children[j] = (*root)->children[j + 1];

      (*root)->s_children--;
      i--;

      if (!(*root)->s_children) {
        free((*root)->children);
        (*root)->children = NULL;
      } else
        (*root)->children = realloc((*root)->children, (*root)->s_children * sizeof(Tree *));
    }
  }
}

void tree_library_delete_tree(Tree **root, void free_state(void *state)) {
  if (!root)
    return;
  if (!*root)
    return;

  for (size_t i = 0; i < (*root)->s_children; i++)
    tree_library_delete_tree(&((*root)->children[i]), free_state);
  if ((*root)->children)
    free((*root)->children);
  if (free_state)
    free_state((*root)->state);
  free(*root);

  *root = NULL;
}

// ======================================# PRIVATE #=========================================

Tree **_tree_recursion(Tree *node, Tree **leaves, size_t *size) {
  if (!node)
    return NULL;

  if (!tree_library_is_leaf(node)) {
    for (size_t i = 0; i < node->s_children; i++) {
      leaves = _tree_recursion(node->children[i], leaves, size);
      if (!leaves)
        break;
    }
    return leaves;
  }

  Tree **temp = (Tree **)realloc(leaves, (*size+1)*sizeof(Tree *));
  if (!temp)
    return NULL;

  leaves = temp;
  leaves[(*size)++] = node;

  return leaves;
}
