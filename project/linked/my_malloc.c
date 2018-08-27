#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "my_malloc.h"

#define META_SIZE sizeof(list_t)

void *head = NULL;

/*
* Merge adjacent blocks
*/
void merge_list() {
  list_t *p, *q;
  p = head;
  if (!head) {
    return;
  }
  while (p->next) {
    if (p->vacant && p->next->vacant) {
      p->size += p->next->size + META_SIZE;
      p->next = p->next->next;
    } else {
      q = p;
      p = p->next;
    }
  }
}

/*
* Find first free block of a big enough size
*/
list_t *first_free_block(list_t **last, size_t size) {
  list_t *p = head;
  while (p && !(p->vacant && p->size >= size)) {
    *last = p;
    p = p->next;
  }
  return p;
}

/*
* Create new block
*/
list_t *create_block(list_t *tail, size_t size) {
  list_t *new_block;
  new_block = sbrk(0);
  void *req = sbrk(size + META_SIZE);
  if (req == (void*) - 1) {
    return NULL;
  }
  if (tail) {
    tail->next = new_block;
  }
  new_block->size = size;
  new_block->next = NULL;
  new_block->vacant = 0;
  return new_block;
}

void free(void *ptr) {
  if (!ptr) {
    return;
  }
  list_t *p = (list_t *) ptr - 1;
  p->vacant = 1;
  merge_list();
}

void *malloc(size_t size) {
  list_t *p;
  if (size <= 0) {
    return NULL;
  }

  if (!head) {
    p = create_block(NULL, size);
    if (!p) {
      return NULL;
    }
    head = p;
  } else {
    list_t *tail = head;
    p = first_free_block(&tail, size);

    if (!p) { //Failed to find a free block
      p = create_block(tail, size);
      if (!p) {
        return NULL;
      }
    } else {
      p->vacant = 0;
    }
  }
  return (p + 1);
}

void *calloc(size_t nitems, size_t size) {
  void *p = malloc(nitems*size);
  if (p != NULL) {
    memset(p, 0, nitems*size);
  }
  return p;
}

void *realloc(void *ptr, size_t size) {
  if (!ptr) {
    return malloc(size);
  }

  list_t *p = (list_t *) ptr - 1;
  if (p->size >= size) {
    return ptr;
  }
  void* n;
  n = malloc(size);
  if (n) {
    return NULL;
  }
  memcpy(n, ptr, p->size);
  free(ptr);
  return n;
}
