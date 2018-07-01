#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct list_t list_t;

void *head = NULL;

struct list_t {
  list_t *next;
  size_t size;
  int vacant;
};

void print_list() {
  if (!head) {
    fprintf(stderr, "The list is empty!");
    return;
  }
  list_t *p = (list_t *)head;
  size_t i = 0;
  while (p) {
    fprintf(stderr, "node: %d have value: %d\n", i, *(int *)(p + 1));
    i++;
    p = p->next;
  }
}

void merge_list() {
  list_t *p, *q;
  p = head;
  if (!head) {
    return;
  }

  while (p->next) {
    if (p->vacant && p->next->vacant) {
      p->size += p->next->size + sizeof(list_t);
      p->next = p->next->next;
    } else {
      q = p;
      p = p->next;
    }
  }
}

list_t *first_free_block(list_t **last) {
  list_t *p = head;
  while (p && !p->vacant) {
    *last = p;
    p = p->next;
  }
  return p;
}

list_t *create_block(list_t *tail, size_t size) {
  list_t *new_block = sbrk(0);
  void *req = sbrk(size + sizeof(list_t));
  if (!req) {
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

void my_free(void *node) {
  if (!node) {
    return;
  }
  list_t *p = ((list_t *)node - 1);
  p->vacant = 1;
  merge_list();
}

void *my_malloc(size_t size) {
  list_t *p;
  if (!head) {
    p = create_block(NULL, size);
    if (!p) {
      return NULL;
    }
    head = p;
  } else {
    list_t *tail = head;
    p = first_free_block(&tail);

    if (!p) {
      p = create_block(tail, size);
      if (!p) {
        return NULL;
      }
    } else {
      p->vacant = 0;
    }
  }
  return p + 1;
}

void *my_calloc(size_t nitems, size_t size) {
  list_t *p = my_malloc(nitems*size);
  if (p != NULL) {
    memset(p, 0, nitems*size);
  }
  return p;
}

void *my_realloc(void *node, size_t size) {
  list_t *p = my_malloc(size);
  if (p != NULL) {
    memmove(p, node, size);
    my_free(node);
  }
  return p;
}
