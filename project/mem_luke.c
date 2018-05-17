#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>


list_t* head = NULL;

typedef struct list_t list_t;

struct list_t {
  list_t next;
  size_t size;
  bool vacant;
};

list_t* first_free_slot()
{
  if (head == NULL) {
    extend_list(head);
    return head;
  }

  list_t* p = head;
  while (!p->vacant) {
    if (p->next == NULL) {
      extend_list(p);
    }
    p = p->next;
  }
  return p;

}

void extend_list(list_t* tail)
{
  list_t* p = tail;
  for (size_t i = 0; i < 5; i++) {
    p->next = create_slot();
    p = p->next;
  }
}

list_t* create_slot()
{
  list_t* new_block;
  new_block->size = 0;
  new_block->next = NULL;
  new_block->vacant = 1;
  return new_block;
}

list_t* create_block(list_t slot, size_t size)
{
  list_t* new_block = sbrk(size + sizeof(list_t));
  new_block->size = size;
  new_block->vacant = 0;
  new_block->next = slot->next;
  return new_block;
}


void* malloc(size_t size)
{
  list_t* p = first_free_slot(size);
  list_t* new_block = create_block(p, size);

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
  list_t* p = first_free_block(size);

  printf("ola");
  return q;
}


void free(list_t* node) {

}

int main(){
  printf("hallo han ar ju retarderad^^");
}
