#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct list_t list_t;

list_t* head = NULL;

struct list_t {
  list_t next;
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
  new_block->vacant = 1;
  return new_block;
}


void* malloc(size_t size)
{
  list_t* next = first_free_block();
  next = create_block(size);

  printf("ola");
  return q;
}


void* calloc(size_t size)
{
  void* p;
  p = sbrk(size);
  if (p < 0) {
    return NULL;
  }
  list_t* p = first_free(size);

  printf("ola");
  return q;
}

void* realloc(size_t size)
{
  void* p;
  p = sbrk(size);
  if (p < 0) {
    return NULL;
  }
  list_t* p = first_free(size);

  printf("ola");
  return q;
}


void free(list_t* node) {

}

void extend_list() {
  
  list_t* p = head->next;
  for (size_t i = 0; i<5; i++) {
    p = create_block;
    p = p->next;
  }
}

int main(){
  printf("hallo han ar ju retarderad^^");
}
