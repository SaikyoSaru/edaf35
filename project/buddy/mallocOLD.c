#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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
  printf("size of node:%zu\n", size);
  return node;
}

node_t* split(node_t* node, size_t size)
{
  printf("split\n");
  if (node->size / 2 < size) {
    printf ("split done, size of block:%d\n", node->size );
    return node;
  } else {
    node->vacant = 0;
    printf ("create left\n");
    node->left = create_node(node, (node->size)/2);
    if (!node->left ) {
      printf("aiuds\n");
    }
    printf("create right\n");
    node->right = create_node(node + (node->size), (node->size));
    return split(node->left, size);
  }
}
/*
* merge two adjacent blocks
*/
void merge_buddies(node_t* left, node_t* right)
{
  left->vacant = 1;
  left->size += right->size;
  left->right = right->right;
  if (right->right != NULL) {
    right->right->left = left;
  }
}

void merge(node_t * node)
{
  if (node == NULL) {
    printf("NODE IS NULL\n");
    return;
  } else if (node->vacant) {
    printf("NODE IS VACANT\n: %d", node->vacant);
    return;
  }

  if (node->left != NULL && node->left->vacant) {
    printf("LEFT NODE IS NOT NULL\n");
    merge(node->left);
  }
  if (node->right != NULL && node->right->vacant) {
    printf("RIGHT NODE IS NOT NULL\n");
    merge(node->right);
  }
  if (node->left->vacant && node->right->vacant) {
    printf("MERGING\n");
    merge_buddies(node->left, node->right);
  }
}


node_t* find_free_spot(node_t* node, size_t size)
{
  printf("find free spot\n");
  if (node->vacant && node->size >= size) {
    printf("Found VACANT\n");
    return node;
  }
  printf("node: %d vacant: %d\n", size, node->vacant);
  if (node->left != NULL && node->left->size >= size) {
    printf("FIND LEFT NODE");
    return find_free_spot(node->left, size);
  }
  if (node->right != NULL && node->right->size >= size){
    printf("FIND RIGHT NODE");
    return find_free_spot(node->right, size);
  }
  // printf("FAIL to find free spot\n");
  return NULL;
}

node_t* create_block(node_t* node, size_t size)
{
  printf("create block\n");
  node_t* p = find_free_spot(root, size);
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
  merge(root);
  // ((node_t*)node)->vacant = 1;
}

void* malloc(size_t size)
{
  /*
  * Allocate in power of 2
  * Our node_t is 32 bytes
  */
  printf("malloc, desired size:%zu\n", size);
  // int n = log(sizeof(node_t)) / log(2);
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
