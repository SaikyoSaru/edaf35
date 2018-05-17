#include <stdio.h>
#include <unistd.h>

typedef struct list_t list_t;
typedef struct list_node list_node;

struct list_node {
  list_node* prev;
  list_node* next;
};


struct list_t {
  list_node node;
  size_t size;
  void * block;
};

list_node
void* malloc(size_t size)
{
  void* p;
  p = sbrk(size);
  if (p < 0) {
    return NULL;
  }

  printf("ola");
  return q;
}

void free(list_t* node) {

}


int main(){
  printf("hallo han ar ju retarderad^^");
}
