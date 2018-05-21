#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

typedef struct list_t list_t;

void* head = NULL;

struct list_t {
  list_t* next;
  size_t size;
  int vacant;
};

list_t* first_free_block(list_t** last)
{
  list_t* p = head;
  while (p && !p->vacant) {
    *last = p;
    p = p->next;
  }
  return p;
}

list_t* create_block(list_t* tail, size_t size)
{
  list_t* new_block = sbrk(0);
  void* req = sbrk(size + sizeof(list_t));
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

void free(void* node) {
  if (!node) {
    return;
  }
  list_t* p = ((list_t*)node-1);
  p->vacant = 1;
}

void* malloc(size_t size)
{
  list_t* p;
  if (!head) {
    printf("head\n");
    p = create_block(NULL, size);
    if (!p) {
      return NULL;
    }
    head = p;
  } else {
    list_t* tail = head;
    p = first_free_block(&tail);

    if (!p) {
      p = create_block(tail, size);
      if (!p){
        return NULL;
      }
    } else {
      p->vacant = 0;
    }
  }
  return p+1;
}


void* calloc(size_t size)
{
  list_t* p = malloc(size);
  if (p != NULL) {
    memset(p, 0, size);
  }
  return p;
}

void* realloc(void* node, size_t size)
{
  list_t* p = malloc(size);
  if (p != NULL) {
    memmove(p, node, size);
    free(node);
  }
  return p;
}

int main(){

  int* test = malloc(sizeof(int));
  printf("test 1 malloc: %d at address: %d\n", *test, test);
  printf("done1\n");
  int* test2 = malloc(sizeof(int));
  printf("test 2 malloc: %d at address: %d\n", *test2, test2);
  printf("done2\n");
  int* test3 = malloc(sizeof(int));
  printf("done3\n");

  *test = 123;
  *test2 = 321;
  *test3 = 213;

  //printf("test head: %d at address: %d\n", head[0], &head);

  printf("test 1 malloc: %d at address: %d\n", *test, test);
  printf("test 2 malloc: %d at address: %d\n", *test2, test2);
  printf("test 3 malloc: %d at address: %d\n", *test3, test3);

  free(test2);
  int* test4 = calloc(sizeof(int));
  *test4 = 666;
  printf("test 1 malloc: %d at address: %d\n", *test, test);
  printf("test 4 malloc: %d at address: %d\n", *test4, test4);
  printf("test 3 malloc: %d at address: %d\n", *test3, test3);
//  printf("test calloc: %d at address: %d\n", test2, &test2);

//  test = realloc(test, 4*sizeof(int));
//  printf("test realloc: %d at address: %d\n", test, &test);
}
