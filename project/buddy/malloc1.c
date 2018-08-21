#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "malloc.h"
#include <math.h>


// typedef struct node_t node_t;
#define INIT_SIZE 512

void* root = NULL;



void* init(size_t size)
{
  printf("Init\n");
  node_t* p = sbrk(0);
  void* req = sbrk(2*size + sizeof(node_t));
  if (!req) {
    return NULL;
  }
  p->pre = NULL;
  p->succ = NULL;
  p->vacant = 1;
  p->size = size;
  printf("init done\n");
  return p;
}

node_t* create_node(node_t* node, size_t size)
{
  printf ("create node\n");
  node->pre = NULL;
  node->succ = NULL;
  node->vacant = 1;
  node->size = size;
  printf("size of node:%zu\n", size);
  return node;
}

node_t* split(node_t* node, size_t size)
{
  printf("split\n");
  node_t* left;
  node_t* right;
  while (node->size / 2 > size) {
    left = node;
    left->size = node->size / 2;
    right = create_node((node_t*) ((char*) node + node->size / 2), node->size / 2);
    left->succ = right;
    right->pre = left;
    right->succ  = node->succ;
    if (!node->succ) {
      node->succ->pre = right;
    }
    node = left;
  }
  return node;
}
/*
* merge two adjacent blocks
*/
void merge_buddies(node_t* left, node_t* right)
{
  left->vacant = 1;
  left->size += right->size;
  left->succ = right->succ;
  if (right->succ != NULL) {
    right->succ->pre = left;
  }
}

void merge(node_t * node)
{
  printf("merge\n");
  if ((node->size == node->succ->size && (!node->vacant || !node->succ->vacant)) || node->succ == NULL) {
    return;
  }
  if (node->size == node->succ->size && node->vacant && node->succ->vacant) {
    merge_buddies(node, node->succ);
    merge(root);
  }
  if (node->size != node->succ->size) {
    merge(node->succ);
  }
}


node_t* find_free_spot(size_t size)
{
  printf("find free spot\n");
  node_t* curr_spot = root;
  while (curr_spot != NULL) {
    if (curr_spot->size >= size && curr_spot->vacant) {
      return curr_spot;
    }
    curr_spot = curr_spot->succ;
  }
  return NULL;
}

node_t* create_block(node_t* node, size_t size)
{
  printf("create block\n");
  node_t* p = find_free_spot(size);
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
