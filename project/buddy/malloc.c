#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "malloc.h"
#include <math.h>
#include "list.h"


typedef struct node_t node_t;
#define INIT_SIZE 2048

void* heap;

node_t* freelist[8];

void split(int high_level, int low_level);




void* root = NULL;

struct node_t {
  node_t* left;
  node_t* right;
  size_t size;
  int vacant;
};

/*
*Returns the level
*/
int get_level(int size) {
  switch (size) {
    case 16:
    return 0;
    case 32:
    return 1;
    case 64:
    return 2;
    case 128:
    return 3;
    case 256:
    return 4;
    case 512:
    return 5;
    case 1024:
    return 6;
    case 2048:
    return 7;
  }
}


void init()
{
  for (size_t i = 0; i<8; i++) {
    freelist[i] = NULL;
  }
  printf("Init\n");
  heap = sbrk(0);
  void* req = sbrk(INIT_SIZE);
  if (!req) {
    return;
  }

  node_t* p = (node_*t) heap;
  p = create_node(p, INIT_SIZE);
  freelist[7] = p;
  // return p;
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
/*
*splits until the right size is found
*/
void split(int high_level, int low_level)
{
  printf("split\n");
  for (size_t i = high_level; i>low_level; i++) {
    node_t* node = pop_list(freelist, i);

    node->size/=2;

    node_t* new_node = (node_t*)((char*)node + node->size);
    new_node->size = node->size;
    new_node->vacant = 1;
    new_node->left = node;
    new_node->right = NULL;
    add_last(freelist, i-1, node);
    add_last(freelist, i-1, new_node);
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
  if (s > INIT_SIZE) {
    return NULL;
  }
  // printf("malloc, desired size:%zu\n", size);
  // int n = log(sizeof(node_t)) / log(2);
  int n = (int)ceil(log2(size + sizeof(node_t)));
  // printf("node_t size: %d\n", sizeof(node_t));
  // printf("order of 2: %d\n", n);
  int alloc_size = 2 << (n-1);
  // printf("alloc size:%d\n", alloc_size);

  node_t* p;
  if (!heap) {
    init();
  }

  level = get_level(alloc_size);

  if (freelist[level] != NULL) {
    node_t* node = pop_list(freelist, level);
    node->vacant = 0;
    return node+1;
  } else {
    int closest_free_level = -1;
    for (size_t i = level + 1; i<8; i++) {
      if(freelist[i]!=NULL) {
        closest_free_level = i;
        break;
      }
    }
    split(closest_free_level, level);

  } else {
    //Add on new memory
     void* p = (node_t*) sbrk(0);
     void* req = sbrk(INIT_SIZE);
     if (!req) {
       printf("out of memory!!");
       return NULL;
     }
     p = create_node(p, INIT_SIZE);
     freelist[7] = p;
     split(7, level);
  }

  node_t* node = pop_list(freelist, level);
  node->vacant = 0;
  return node+1;
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
