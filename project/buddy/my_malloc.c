#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "my_malloc.h"

#define META_SIZE sizeof(node_t)
#define MAX_LEVEL 32
#define INIT_SIZE (1L << MAX_LEVEL)

void* heap = NULL;

node_t* freelist[MAX_LEVEL + 1];

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
  for (int i = 0; i <= MAX_LEVEL; i++) {
    freelist[i] = NULL;
  }
  heap = sbrk(INIT_SIZE);
  if (!heap) {
    exit(0);
  }
  node_t* p = (node_t*) heap;
  p->level = MAX_LEVEL;
  p->next = NULL;
  freelist[MAX_LEVEL] = p;
}

void printer(void)
{
  node_t* p;
  for (int i = 0; i < MAX_LEVEL; i++) {
    p = freelist[i];
    while (p != NULL) {
      fprintf(stderr, "%d (%zu),", p->level, p);
      fprintf(stderr, "%d,", p->level);
      p = p->next;
    }
    if (p == NULL) {
      fprintf(stderr, "-");
    }
    fprintf(stderr, "  ");
  }
  fprintf(stderr, "\n");
}

/*
* Inserts node into the freelist
*/
void insert(node_t* node)
{
  node_t* p = freelist[node->level];
  node->next = NULL;
  if (p == NULL || p > node) {
    node->next = p;
    freelist[node->level] = node;
  } else {
    while (p->next != NULL && node > p) {
        p = p->next;
    }
    node->next = p->next;
    p->next = node;
  }
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
  node_t* node = pop(high_level);
  for (size_t i = high_level; i > low_level; i--) {
    size_t new_size = (1L << i - 1);
    node_t* new_node = (node_t*)((char*)node + new_size);
    new_node->level = i-1;
    new_node->next = NULL;
    node->level = i-1;
    node->next = NULL;
    freelist[i-1] = new_node;
  }
  return node;
}

/*
* Merges adjacent buddies to a bigger chunk
*/
void merge(int level)
{
  if (level >= MAX_LEVEL) {
    return;
  }
  node_t* p = freelist[level];
  node_t* q = p;
  while (p->next != NULL) {
    if ((((size_t) p->next) - ((size_t) p)) == 1L << level && (((void*)p - heap) / (1L << level) % 2)) {
      if (((size_t) p) - ((size_t) q) == 0) {
        freelist[level] = p->next->next;
      } else {
        q->next = p->next->next;
      }
      p->level+=1;
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
  if (node == NULL || heap == NULL) {
    return;
  }
  node_t* p = ((node_t*)node) - 1;
  insert(p);
  merge(p->level);
}

void* malloc(size_t size)
{
  if (size > INIT_SIZE - META_SIZE || size <= 0) {
    return NULL;
  }
  int level = get_level(size + META_SIZE);
  node_t* p;
  node_t* node;
  int closest_free_level = -1;

  if (heap == NULL) {
    init();
  }

  for (int i = level; i <= MAX_LEVEL; i++) {
    if (freelist[i] != NULL) {
      closest_free_level = i;
      break;
    }
  }

  if (closest_free_level == -1) {
    return NULL;
  }

  node = split(closest_free_level, level);

  return (void*) (node + 1);
}


void* calloc(size_t nitems, size_t size)
{
  void* p = malloc(nitems*size);
  if (p != NULL) {
    memset(p, 0, nitems*size);
  }
  return p;
}

void* realloc(void* ptr, size_t size)
{
  if (!ptr) {
    return malloc(size);
  }

  if (!size) {
    free(ptr);
    return NULL;
  }

  node_t* q = ((node_t*)ptr) - 1;
  size_t old_size = 1L << q->level;

  if (old_size - META_SIZE >= size) {
    return ptr;
  }

  node_t* p = malloc(size);
  if (p != NULL) {
    memcpy(p, q+1, old_size - META_SIZE);
    free(q+1);
  }

  return p;
}
