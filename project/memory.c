#include <stdio.h>
#include <unistd.h>
static list_node* head;

typedef struct list_t list_t;
typedef struct list_node list_node;

struct list_node {
  list_node* prev;
  list_node* next;
};


struct list_t {
  list_node node;
  size_t size;
  void * block; // predefine blocksize? factor of 2 or 4?
};

list_node* first_free(size_t size)
{
  list_node* p = head;
  while (p->next !=NULL) {
    p = p->next;
  }
// check if the amount of free blocks can accomodate the size

}
void* malloc(size_t size)
{
  void* p;
  p = sbrk(size);
  if (p < 0) {
    return NULL;
  }
  list_node* p = first_free(size);

  printf("ola");
  return q;
}

void free(list_t* node) {

}


int main(){
  printf("hallo han ar ju retarderad^^");
}
