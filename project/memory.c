#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

typedef struct list_t list_t;

list_t* head = NULL;

struct list_t {
  list_t* next;
  size_t size;
};

list_t* first_free_block()
{
  if (head == NULL) {
    return head;
  }
  list_t* p = head;
  while (p->next != NULL) {
    p = p->next;
  }
  return p->next;
}

list_t* create_block(size_t size)
{
  list_t* new_block = sbrk(size + sizeof(list_t));
  new_block->size = size;
  new_block->next = NULL;
  //new_block->vacant = 1;
  return new_block;
}


void* malloc(size_t size)
{
  list_t* next = first_free_block();
  next = create_block(size);
  return next;
}


void* calloc(size_t size)
{
  list_t * p = malloc(size);
  if (p != NULL) {
    memset(p, 0, size);
  }
  return p;
}

void free(list_t* node) {
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

  int* test = malloc(2*sizeof(int));
  int* test2 = calloc(2*sizeof(int));



  printf("test malloc: %d at address: %d\n", test[0], &test[0]);
  printf("test calloc: %d at address: %d\n", test2[0], &test2[0]);

  test = realloc(test, 4*sizeof(int));
  printf("test realloc: %d at address: %d\n", test[0], &test[0]);





}
