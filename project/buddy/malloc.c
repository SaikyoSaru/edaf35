// #include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "malloc.h"
#include <math.h>
#include "list.h"


#define INIT_SIZE 2048

void* heap;

node_t* freelist[8];

void split(int high_level, int low_level);

// void* root = NULL;

/*
*Returns the level
*/
int get_level(int size) {
  switch (size) {
    case 16:
    return 0;
    case 32:
    return 1;
    case 64:
    return 2;
    case 128:
    return 3;
    case 256:
    return 4;
    case 512:
    return 5;
    case 1024:
    return 6;
    case 2048:
    return 7;
  }
}


node_t* create_node(node_t* node, size_t size)
{
  printf ("create node\n");
  node->pre = NULL;
  node->succ = NULL;
  node->vacant = 1;
  node->size = size;
  printf("size of node:%zu\n", size);
  return node;
}

void init()
{
  for (size_t i = 0; i<8; i++) {
    freelist[i] = NULL;
  }
  printf("Init\n");
  heap = sbrk(0);
  void* req = sbrk(INIT_SIZE);
  if (!req) {
    return;
  }

  node_t* p = (node_t*) heap;
  p = create_node(p, INIT_SIZE);
  freelist[7] = p;
  // return p;
}

/*
*splits until the succ size is found
*/
void split(int high_level, int low_level)
{
  printf("split\n");
  for (size_t i = high_level; i>low_level; i++) {
    node_t* node = pop_list(freelist, i);

    node->size/=2;

    node_t* new_node = (node_t*)((char*)node + node->size);
    new_node->size = node->size;
    new_node->vacant = 1;
    new_node->pre = node;
    new_node->succ = NULL;
    add_last(freelist, i-1, node);
    add_last(freelist, i-1, new_node);
  }
}
/*
* merge two adjacent blocks
*/
void merge_buddies(node_t* pre, node_t* succ)
{
  pre->vacant = 1;
  pre->size += succ->size;
  pre->succ = succ->succ;

  if (succ->succ != NULL) {
    succ->succ->pre = pre;
  }
}

void merge(node_t * node)
{
  if (node == NULL) {
    printf("NODE IS NULL\n");
    return;
  } else if (node->vacant) {
    printf("NODE IS VACANT\n: %d", node->vacant);
    return;
  }

  if (node->pre != NULL && node->pre->vacant) {
    printf("pre NODE IS NOT NULL\n");
    merge(node->pre);
  }
  if (node->succ != NULL && node->succ->vacant) {
    printf("succ NODE IS NOT NULL\n");
    merge(node->succ);
  }
  if (node->pre->vacant && node->succ->vacant) {
    printf("MERGING\n");
    merge_buddies(node->pre, node->succ);
  }
}

void free(void* node)
{
  print("free");
  node-=1;
  if (!node) {
    return;
  }

}

void* malloc(size_t size)
{
  /*
  * Allocate in power of 2
  * Our node_t is 32 bytes
  */
  if (size > INIT_SIZE) {
    return NULL;
  }
  printf("malloc, desired size:%zu\n", size);
  // int n = log(sizeof(node_t)) / log(2);
  int n = (int)ceil(log2(size + sizeof(node_t)));
  // printf("node_t size: %d\n", sizeof(node_t));
  // printf("order of 2: %d\n", n);
  int alloc_size = 2 << (n-1);
  // printf("alloc size:%d\n", alloc_size);
  node_t* p;
  if (!heap) {
    init();
  }

  int level = get_level(alloc_size);

  if (freelist[level] != NULL) {
    node_t* node = pop_list(freelist, level);
    node->vacant = 0;
    return node+1;
  } else {
    int closest_free_level = -1;
    for (size_t i = level + 1; i<8; i++) {
      if(freelist[i]!=NULL) {
        closest_free_level = i;
        break;
      }
    }
    if (closest_free_level) {
    split(closest_free_level, level);

    } else {
      //Add on new memory
       void* p = (node_t*) sbrk(0);
       void* req = sbrk(INIT_SIZE);
       if (!req) {
         printf("out of memory!!");
         return NULL;
       }
       p = create_node(p, INIT_SIZE);
       freelist[7] = p;
       split(7, level);
    }
  }
  node_t* node = pop_list(freelist, level);
  node->vacant = 0;
  return node+1;
}


void* calloc(size_t nitems, size_t size)
{
  node_t* p = malloc(nitems*size);
  if (p != NULL) {
    memset(p, 0, nitems*size);
  }
  return p;
}

void* realloc(void* node, size_t size)
{
  node_t* p = malloc(size);
  if (p != NULL) {
    memmove(p, node, size);
    free(node);
  }
  return p;
}

int main(int argc, char const *argv[]) {
  // printf("size: %zu\n", sizeof(int) + sizeof(node_t));
  int* a = malloc(2*sizeof(int));
  // printf("tester:%zu\n", a[]);
  a[0] = 42;
  a[1] = 15;
  printf("a val: %d address:%zu, pointer val: %zu\n", *a, &a, a);
  a += 1;
  printf("a val: %d address:%zu, pointer val: %zu\n", *a, &a, a);
  int* b = calloc(1, sizeof(int));
  *b = 1337;
  printf("b val: %d address:%zu, pointer val: %zu\n", *b, &b, b);
  free(b);
  int* b2 = calloc(1, sizeof(int));
  *b2 = 420;
  printf("b val: %d address:%zu, pointer val: %zu\n", *b2, &b2, b2);
  a -= 1;
  free(a);



}
