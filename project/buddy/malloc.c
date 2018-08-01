#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "malloc.h"
#include <math.h>


typedef struct node_t node_t;
#define INIT_SIZE 512

void* root = NULL;

struct node_t {
  node_t* left;
  node_t* right;
  size_t size;
  int vacant;
};

void* init(size_t size)
{
  printf("Init\n");
  node_t* p = sbrk(0);
  void* req = sbrk(2*size + sizeof(node_t));
  if (!req) {
    return NULL;
  }
  p->left = NULL;
  p->right = NULL;
  p->vacant = 1;
  p->size = size;
  printf("init done\n");
  return p;
}

node_t* create_node(node_t* node, size_t size)
{
  printf ("create node\n");
  node->left = NULL;
  node->right = NULL;
  node->vacant = 1;
  node->size = size;
  return node;
}

node_t* split(node_t* node, size_t size)
{
  printf("split\n");
  if (node->size / 2 < size) {
    printf ("split done, size of block:%d\n", node->size );
    return node;
  } else {
    printf ("create left\n");
    node->left = create_node(node, (node->size)/2);
    if (!node->left ) {
      printf("aiuds\n");
    }
    printf("create right\n");
    node->right = create_node(node + (node->size)/2, (node->size)/2);
    return split(node->left, size);
  }
}
/*
* merge two adjacent blocks
*/
node_t* merge_buddies(node_t* left, node_t* right)
{
  left->vacant = 1;
  left->size += right->size;
  left->right = right->right;
  if (right->right != NULL) {
    right->right->left = left;
  }
  return left;
}

node_t* merge(node_t * node)
{
  return NULL;
}
node_t* find_free_spot(node_t* node, size_t size)
{
  printf("find free spot\n");
  if (node->vacant && node->size >= size) {
    return node;
  }
  if (!node->left && node->left->size >= size && node->left->vacant) {
    return find_free_spot(node->left, size);
  } else if (!node->right && node->right->vacant){
    return find_free_spot(node->right, size);
  }
  return NULL;
}

node_t* create_block(node_t* node, size_t size)
{
  printf("create block\n");
  node_t* p = find_free_spot(node, size);
  if (!p) {
    return NULL;
  }
  p = split(p, size);
  p->vacant = 0;
  return p;
}

void free(void* node)
{
  if (!node) {
    return;
  }
  merge(node);
  ((node_t*)node)->vacant = 1;
}

void* malloc(size_t size)
{
  /*
  * Allocate in power of 2
  * Our node_t is 32 bytes
  */
  printf("malloc, desired size:%zu\n", size);
  int n = (int)ceil(log2(size + sizeof(node_t)));
  printf("node_t size: %d\n", sizeof(node_t));
  printf("order of 2: %d\n", n);
  int alloc_size = 2 << (n-1);
  printf("alloc size:%d\n", alloc_size);

  node_t* p;
  if (!root) {
    root = init(INIT_SIZE);
  }
  p = create_block(root, alloc_size);
  if (!p) {
    printf ("NULL");
    return NULL;
  }
  p->vacant = 1;
  return p+1;
}


void* calloc(size_t nitems, size_t size)
{
  node_t* p = malloc(nitems*size);
  if (p != NULL) {
    memset(p, 0, nitems*size);
  }
  return p;
}

void* realloc(void* node, size_t size)
{
  node_t* p = malloc(size);
  if (p != NULL) {
    memmove(p, node, size);
    free(node);
  }
  return p;
}
