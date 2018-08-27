#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "malloc.h"
#include <math.h>

#define META_SIZE sizeof(node_t)
#define INIT_SIZE 2048 //consideration use pow?
#define MAX_LEVEL 7

void* heap;

node_t* freelist[8];

node_t* split(int high_level, int low_level);

/*
* Returns the level
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

/*
* Iitializes memory chunk to use
*/
void init()
{
  for (size_t i = 0; i<8; i++) {
    freelist[i] = NULL;
  }
  heap = sbrk(0);
  void* req = sbrk(INIT_SIZE);
  if (!req) {
    return;
  }
  node_t* p = (node_t*) heap;
  p->level = MAX_LEVEL;
  p->next = NULL;
  freelist[MAX_LEVEL] = p;
}

/*
* Inserts node into the freelist
*/
void insert(node_t* node)
{
  int level = node->level;
  node_t* p = freelist[level];
  node_t* q;
  if (p) {
    if (&node < &p) {
      node->next = p;
      freelist[level] = node;
    } else if (!p->next) {
      p->next = node;
    } else {
      while (p->next) {
        if (&node > &p->next) {
          q = p;
          p = p->next;
        } else {
          q->next = node;
          node->next = p;
          break;
        }
      }
    }
  } else {
    freelist[level] = node;
  }
  return;
}

/*
* Retrieves memory and removes it from the freelist
*/
node_t* pop(int level)
{
  node_t* p = freelist[level];
  if (p->next == NULL) {
    freelist[level] = NULL;
  } else {
    freelist[level] = p->next;
  }
  p->next = NULL;
  return p;
}

/*
* splits until the right size is found
*/
node_t* split(int high_level, int low_level)
{
  node_t* node = NULL;
  node = pop(high_level);
  for (size_t i = high_level; i > low_level; i--) {
    size_t size = pow(2, i+4-1);
    usleep(1000000);
    node_t* new_node = (node_t*)((char*)node + size);
    new_node->level = i-1;
    node->level = i-1;
    insert(new_node);
  }
  return node;
}

/*
* Merges adjacent buddies to a bigger chunk
*/
void merge(int level)
{
  node_t* p = freelist[level];
  if (p->next == NULL || level > MAX_LEVEL) {
    return;
  }
  node_t* q = p;
  while (p->next) {
    if (!((int)&p % (2*level + 1)) && (&p->next - &p) == pow(2, level)) {
      if (&p - &q == 0) {
        freelist[level] = p->next->next;
      } else {
        q->next = p->next->next;
      }
      p->level+=1; //inc level
      insert(p);
      merge(level + 1);
      return;
    }
    q = p;
    p = p->next;
  }
}

void free(void* node)
{
  node_t* p = (node_t*)node;
  p-=1;
  if (!p) {
    return;
  }
  int level = p->level;
  insert(p);
  merge(level);
}

void* malloc(size_t size)
{
  if (size > INIT_SIZE) {
    return NULL;
  }

  // Allocate in power of 2 Our node_t is 32 bytes
  int n = (int)ceil(log2(size + META_SIZE));
  int alloc_size = 2 << (n-1);
  node_t* p;
  node_t* node;
  int closest_free_level = -1;

  if (!heap) {
    init();
  }

  int level = get_level(alloc_size);
  if (freelist[level] != NULL) {
    node = pop(level);
  } else {
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
         return NULL;
       }
       p->level = MAX_LEVEL;
       p->next = NULL;
       freelist[MAX_LEVEL] = p;
       closest_free_level = MAX_LEVEL;
    }
    node = split(closest_free_level, level);
  }
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
