#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <stdio.h>

void *realloc(void *ptr, size_t size);
void *malloc(size_t size);
void *calloc(size_t nitems, size_t size);
void free(void* ptr);

typedef struct node_t node_t;

struct node_t {
  int level;
  node_t* next;
};

#endif
