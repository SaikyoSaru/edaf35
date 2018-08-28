#include <stdio.h>
#include <unistd.h>
// #include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "my_malloc.h"
#include <math.h>

#define META_SIZE sizeof(node_t)
#define MAX_LEVEL 20
#define INIT_SIZE 1048576

void* heap;

node_t* freelist[MAX_LEVEL - 1];

node_t* split(int high_level, int low_level);

/*
* Returns the level
*/
int get_level(int size) {
  return (int)ceil(log2(size));
}

/*
* Iitializes memory chunk to use
*/
void init()
{
  for (size_t i = 0; i < MAX_LEVEL; i++) {
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
  //fprintf(stderr, "INSERT level:%d\n", level);
  node_t* p = freelist[level];
  node_t* q;
  if (p) {
    if (node < p) {
      node->next = p;
      freelist[level] = node;
    } else if (!p->next) {
      p->next = node;
    } else {
      while (p->next) {
        if (node > p->next) {
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
  //fprintf(stderr, "%d\n", level);
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
  //fprintf(stderr, "SPLIT\n");
  node_t* node = NULL;
  node = pop(high_level);
  for (size_t i = high_level; i > low_level; i--) {
    //fprintf(stderr, "level: %d\n", i);
    printer();
    size_t size = 1 << i; // pow(2, i+4-1);
    node_t* new_node = (node_t*)((char*)node + size);
    //fprintf(stderr, "node: %zu, new_node: %zu\n", node, new_node);
    new_node->level = i-1;
    new_node->next = NULL;
    node->level = i-1;
    node->next = NULL;
    insert(new_node);
  }
  return node;
}

void printer(void)
{
  node_t* p;
  for (int i = 0; i <= MAX_LEVEL; i++) {
    p = freelist[i];
    while (p != NULL) {
      //fprintf(stderr, "%d (%zu),", p->level, p);
      p = p->next;
    }
    if (p == NULL) {
      //fprintf(stderr, "-");
    }
    //fprintf(stderr, "  ");
  }
  //fprintf(stderr, "\n");
}

/*
* Merges adjacent buddies to a bigger chunk
*/
void merge(int level)
{
  //fprintf(stderr, "Merge level:%d\n", level);
  node_t* p = freelist[level];
  if (p->next == NULL || level >= MAX_LEVEL) {
    //fprintf(stderr, "RETURN\n");
    return;
  }
  node_t* q = p;
  while (p->next) {
    if ((((size_t) p->next) - ((size_t) p)) == 1 << level) {
      if (((size_t) p) - ((size_t) q) == 0) {
        freelist[level] = p->next->next;
      } else {
        q->next = p->next->next;
      }
      p->level+=1; //inc level
      p->next = NULL;
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
  if (node == NULL) {
    return;
  }
  node_t* p = (node_t*)node;
  p-=1;
  //fprintf(stderr, "FREE %d\n", p->level);
  if (p == NULL) {
    return;
  }
  int level = p->level;
  printer();
  insert(p);
  printer();
  merge(level);
  printer();
}

void* malloc(size_t size)
{
  //fprintf(stderr, "MALLOC\n");
  //fprintf(stderr, "size: %zu\n", size);
  if (size > INIT_SIZE - META_SIZE) {
    //fprintf(stderr, "COMEON\n");
    return NULL;
  }
  printer();
  // Allocate in power of 2 Our node_t is 32 bytes
  int level = get_level(size + META_SIZE);
  //fprintf(stderr, "level for alloc shit:%d\n", level);
  // int alloc_size = 2 << (n-1);
  node_t* p;
  node_t* node;
  int closest_free_level = -1;

  if (!heap) {
    init();
  }

  // int level = get_level(alloc_size);
  if (freelist[level] != NULL) {
    node = pop(level);
  } else {
    for (size_t i = level + 1; i < MAX_LEVEL; i++) {
      if(freelist[i] != NULL) {
        closest_free_level = i;
        break;
      }
    }
    if (closest_free_level == -1) {
      //Add on new memory
      //fprintf(stderr, "MOOOOORE\n");
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
  printer();
  return (node+1);
}


void* calloc(size_t nitems, size_t size)
{
  //fprintf(stderr, "CALLOC\n");
  if (nitems == 0 || size == 0) {
    return NULL;
  }
  node_t* p = malloc(nitems*size);
  if (p != NULL) {
    memset(p, 0, nitems*size);
  }
  return p;
}

void* realloc(void* node, size_t size)
{
  //fprintf(stderr, "REALLOC\n");
  if (!node) {
    return malloc(size);
  }
  node_t* q = (node_t*)node;
  int old_size = 2 << ((q-1)->level + 4 - 1);
  // printf("size of transfer %d:\n", old_size);
  //
  // printf("Old, point at values:%d, %zu, %zu\n", *q, &q, q);
  node_t* p = malloc(size);
  // printf("New :%d, %zu, %zu\n", *p, &p, p);
  if (p != NULL) {
    memcpy(p, q, old_size - META_SIZE);
    free(q);
  }
  // printf("Return values :%d, %zu, %zu\n", *p, &p, p);

  return p;
}
