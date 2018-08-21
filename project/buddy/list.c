


node_t* pop_list(node_t** freelist, int level) {
  node_t* p = freelist[level];
  freelist[level] = p->next;

  if ((node_t*)freelist[level] !=NULL ) {
    (node_t*)freelist[level]->left = NULL;
  }
  p->left = NULL;
  p->right = NULL;
  return p;
}


/*
* traverse list and put free blocks last in the given level
*/
node_t* add_last(node_t** freelist, int level, node_t* node) {
  node_t* p = freelist[level];

  if (p == NULL) {
    freelist[level] = node;
    node->left = NULL;
  } else {
    node_t* q = p;
    while (q->next != NULL) {
      q = q->right;
    }
    q->next = node;
    node->left = q;
  }
  node->right = NULL;
  return node;
}

node_t* remove_node(node_t** freelist, int level, node_t* node) {
  if (node->left == NULL && node->right==NULL) {
    freelist[level] = NULL;
  } else if (node->left == NULL) {
    freelist[level] = node->next;
    node->next->prev = NULL;
  } else if (node->right = NULL) {
    node->left->right = NULL;
  } else {
    node->left->right = node->rigth;
    node->right->left = node->left;
  }
  return node:
}
