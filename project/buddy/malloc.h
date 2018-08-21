#ifndef _MALLOC_H
#define _MALLOC_H

#include <stdio.h>

void *realloc(void *ptr, size_t size);
void *malloc(size_t size);
void *calloc(size_t nitems, size_t size);
void free(void* ptr);

typedef struct node_t node_t;

struct node_t {
  node_t* pre;
  node_t* succ;
  size_t size;
  int vacant;
};

#endif
