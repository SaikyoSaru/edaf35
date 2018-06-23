#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
//#include <stdlib.h>
typedef struct list_t list_t;

list_t* head = NULL;

struct list_t {
  list_t* next;
  list_t* prev;
  size_t size;
};

list_t* first_free_block()
{
  list_t* p = head;
  while (p && p->next != NULL) {
    p = p->next;
    printf("fuck");
  }
  return p;
}

list_t* create_block(size_t size)
{
  list_t* new_block = sbrk(0);
  list_t* req = sbrk(size + sizeof(list_t));
  if (!req) {
    return NULL;
  }
  new_block->size = size;
  new_block->next = NULL;
  new_block->prev = NULL;
  return new_block;
}

void my_free(void* node) {
  if (!node) {
    return;
  }
  list_t* p = ((list_t*)node-1);
}

void* my_malloc(size_t size)
{
  list_t* p = create_block(size);
  if (head == NULL) {
    printf("head\n");
    head = p;
  } else {
    list_t* current = first_free_block();
//    fprintf(stderr, "current: %d at address: %d\n", *current, current);
    p->prev = current;
    current->next = p;
  }
  return p+1;
}


void* my_calloc(size_t size)
{
  list_t* p = my_malloc(size);
  if (p != NULL) {
    memset(p, 0, size);
  }
  return p;
}


void print_list() {
  if (!head) {
    printf("The list is empty!");
    return;
  }
  list_t* p = (list_t*)head;
  size_t i = 0;
  while (p) {
    fprintf(stderr, "node: %d have value: %d\n", i, *(int*)(p+1));
    i++;
    p = p->next;
  }
}

void* my_realloc(void* node, size_t size)
{
  list_t* p = my_malloc(size);
  if (p != NULL) {
    memmove(p, node, size);
    my_free(node);
  }
  return p;
}

//int main(){

 // int* test = my_calloc(sizeof(int));
  //fprintf(stderr, "test 1 malloc: %d at address: %d\n", *test, test);
 // fprintf(stderr, "done1\n");
 // int* test2 = my_calloc(sizeof(int));
 // fprintf(stderr, "test 2 malloc: %d at address: %d\n", *test2, test2);
 // fprintf(stderr, "done2\n");
 // int* test3 = my_calloc(sizeof(int));
  //fprintf(stderr, "done3\n");


 // *test = 123;
 // *test2 = 321;
 // *test3 = 213;


  //printf("test head: %d at address: %d\n", head[0], &head);

//  fprintf(stderr, "test 1 malloc: %d at address: %d\n", *test, test);
//  fprintf(stderr, "test 2 malloc: %d at address: %d\n", *test2, test2);
//  fprintf(stderr, "test 3 malloc: %d at address: %d\n", *test3, test3);
//  print_list();

 // my_free(test2);
  //int* test4 = calloc(sizeof(int));
  //*test4 = 100;
//  fprintf(stderr, "test 1 malloc: %d at address: %d\n", *test, test);
  //printf("test 4 malloc: %d at address: %d\n", *test4, test4);
 // fprintf(stderr, "test 3 malloc: %d at address: %d\n", *test3, test3);
//  printf("test calloc: %d at address: %d\n", test2, &test2);

//  test = realloc(test, 4*sizeof(int));
//  printf("test realloc: %d at address: %d\n", test, &test);
//}
