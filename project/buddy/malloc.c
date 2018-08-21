#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "malloc.h"
#include <math.h>
// #include "list.h"


#define INIT_SIZE 2048 //consideration use pow?
#define MAX_LEVEL 7

void* heap;

node_t* freelist[8];

node_t* split(int high_level, int low_level);

// void* root = NULL;

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


// void create_node(node_t* node, size_t size)
// {
//   printf ("create node\n");
//   node->pre = NULL;
//   node->succ = NULL;
//   node->vacant = 1;
//   node->size = size;
//   printf("size of node:%zu\n", size);
//
// }

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

  node_t* p = (node_t*) heap;
  p->level = MAX_LEVEL;
  p->next = NULL;
  // create_node(p, INIT_SIZE);
  printf("%p", p);
  freelist[MAX_LEVEL] = p;
  // return p;
}

void insert(node_t* node, int level) {
  node_t* p = freelist[level];
  node_t* q;
  if (!p) {
    while (!p->next) {
      if (&p < &p->next) {
        q = p;
        p = p->next;
      }
    }
    q->next = node;
    node->next = p;
  }
  freelist[level] = node;
  return;

}

/*
*splits until the succ size is found
*/
node_t* split(int high_level, int low_level)
{
  node_t* node = NULL;
  for (size_t i = high_level; i > low_level; i--) {
    // node_t* node = pop_list(freelist, i);
    node = freelist[i]->node;
    if (!freelist[i]->node->next) {
        freelist[i] = NULL;
    } else {
      freelist[i]->node = freelist[i]->node->next;
    }

    // node->size/=2;
    size_t size = pow(2, i-1);

    node_t* new_node = (node_t*)((char*)node + size);
    new_node->level = i-1;
    node->level = i-1;
    freelist[i-1] = insert(new_node);

    // new_node->size = node->size;
    // new_node->vacant = 1;
    // new_node->pre = node;
    // new_node->succ = NULL;
    // add_last(freelist, i-1, node);
    // add_last(freelist, i-1, new_node);
  }
  return node;
}



void merge(node_t* node)
{
  printf("merging\n");
  // if (node->level == MAX_LEVEL) {
  //   return;
  // }
  // if (!freelist[level]) {
  //   freelist[level] = node;
  // } else {
  //
  //
  // }

}

void free(void* node)
{
  printf("free\n");
  node-=1;
  if (!node) {
    return;
  }
  merge(node);

}

void* malloc(size_t size)
{
  /*
  * Allocate in power of 2
  * Our node_t is 32 bytes
  */
  if (size > INIT_SIZE) {
    return NULL;
  }
  printf("malloc, desired size:%zu\n", size);
  // int n = log(sizeof(node_t)) / log(2);
  int n = (int)ceil(log2(size + sizeof(node_t)));
  // printf("node_t size: %d\n", sizeof(node_t));
  // printf("order of 2: %d\n", n);
  int alloc_size = 2 << (n-1);
  // printf("alloc size:%d\n", alloc_size);
  node_t* p;
  node_t* node;
  int closest_free_level = -1;

  if (!heap) {
    init();
  }

  int level = get_level(alloc_size);

  if (freelist[level] != NULL) {
    // node_t* node = pop_list(freelist, level);
    node = freelist[level];
    // node->vacant = 0;
    // return node+1;
  } else {
    closest_free_level = -1;
    for (size_t i = level + 1; i < 8; i++) {
      if(freelist[i] != NULL) {
        closest_free_level = i;
        break;
      }
    }
    if (!closest_free_level) {
      //Add on new memory
       p = (node_t*) sbrk(0);
       void* req = sbrk(INIT_SIZE);
       if (!req) {
         printf("out of memory!!\n");
         return NULL;
       }
       // create_node(p, INIT_SIZE);
       p->level = MAX_LEVEL;
       p->next = NULL;
       freelist[MAX_LEVEL] = p;
       closest_free_level = MAX_LEVEL;
    }
    node = split(closest_free_level, level);

  }
  // node_t* node = pop_list(freelist, level);
  // node->vacant = 0;
  // node_t* node = freelist[level];
  return (node+1);
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
