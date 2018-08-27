#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <stdio.h>

void *realloc(void *ptr, size_t size);
void *malloc(size_t size);
void *calloc(size_t nitems, size_t size);
void free(void* ptr);

typedef struct list_t list_t;

struct list_t {
  list_t *next;
  size_t size;
  int vacant;
};

#endif
