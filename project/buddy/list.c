#include "list.h"


node_t* pop_list(node_t** freelist, int level) {
  node_t* p = freelist[level];
  if (p->succ != NULL) {
    freelist[level] = p->succ;
  } else {
    freelist[level] = NULL;
  }

  if ((node_t*)freelist[level] != NULL ) {
    ((node_t*)freelist[level])->pre = NULL;
  }
  p->pre = NULL;
  p->succ = NULL;
  return p;
}


/*
* traverse list and put free blocks last in the given level
*/
node_t* add_last(node_t** freelist, int level, node_t* node) {
  node_t* p = freelist[level];

  if (p == NULL) {
    freelist[level] = node;
    node->pre = NULL;
  } else {
    node_t* q = p;
    while (q->succ != NULL) {
      q = q->succ;
    }
    q->succ = node;
    node->pre = q;
  }
  node->succ = NULL;
  return node;
}

node_t* remove_node(node_t** freelist, int level, node_t* node) {
  if (node->pre == NULL && node->succ==NULL) {
    freelist[level] = NULL;
  } else if (node->pre == NULL) {
    freelist[level] = node->succ;
    node->succ->pre = NULL;
  } else if (node->succ == NULL) {
    node->pre->succ = NULL;
  } else {
    node->pre->succ = node->succ;
    node->succ->pre = node->pre;
  }
  return node;
}
