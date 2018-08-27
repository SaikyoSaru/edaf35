#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "malloc.h"
#include <math.h>
// #include "list.h"


#define INIT_SIZE 2048 //consideration use pow?
#define MAX_LEVEL 7

void* heap;

node_t* freelist[8];

node_t* split(int high_level, int low_level);

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


// void create_node(node_t* node, size_t size)
// {
//   printf ("create node\n");
//   node->pre = NULL;
//   node->succ = NULL;
//   node->vacant = 1;
//   node->size = size;
//   printf("size of node:%zu\n", size);
//
// }

void init()
{
  for (size_t i = 0; i<8; i++) {
    freelist[i] = NULL;
  }
  printf("Init\n");
  heap = sbrk(0);
  void* req = sbrk(INIT_SIZE);
  if (!req) {
    fprintf(stderr, "FUUUUCK\n");
    return;
  }

  node_t* p = (node_t*) heap;
  p->level = MAX_LEVEL;
  p->next = NULL;
  // create_node(p, INIT_SIZE);
  fprintf(stderr, "init level %d\n", p->level);
  freelist[MAX_LEVEL] = p;
  // return p;
}

void insert(node_t* node)
{
  int level = node->level;
  node_t* p = freelist[level];
  node_t* q;
  if (p) {
    if (&node < &p) {
      node->next = p;
      freelist[level] = node;
    } else if (!p->next) {
      p->next = node;
    } else {
      while (p->next) {
        if (&node > &p->next) {
          q = p;
          p = p->next;
        } else {
          q->next = node;
          node->next = p;
          break;
        }
      }
    }
  } else {
    freelist[level] = node;
    fprintf(stderr, "YESSSSSS: %d\n", level);
  }
  return;
}

node_t* pop(int level)
{
  node_t* p = freelist[level];
  // if (p == NULL) {
  //   return NULL;
  // }
  if (p->next == NULL) {
    freelist[level] = NULL;
  } else {
    freelist[level] = p->next;
  }
  p->next = NULL;
  return p;
}

/*
*splits until the succ size is found
*/
node_t* split(int high_level, int low_level)
{
  node_t* node = NULL;
  node = pop(high_level);
  for (size_t i = high_level; i > low_level; i--) {
    // node_t* node = pop_list(freelist, i);
    size_t size = pow(2, i+4-1);
    fprintf(stderr, "size: %d\n", size);
    fprintf(stderr, "level: %d\n", node->level-1);
    usleep(1000000);
    node_t* new_node = (node_t*)((char*)node + size);
    new_node->level = i-1;
    node->level = i-1;
    fprintf(stderr, "levels:%d %d\n", node->level, new_node->level);
    insert(new_node);
  }
  return node;
}

void merge(int level)
{
  node_t* p = freelist[level];
  if (p->next == NULL || level > MAX_LEVEL) {
    fprintf(stderr, "NOTHING TO MERGE WITH\n");
    return;
  }
  node_t* q = p;
  while (p->next) {
    if (!((int)&p % (2*level + 1)) && (&p->next - &p) == pow(2, level)) {
      if (&p - &q == 0) {
        freelist[level] = p->next->next;
      } else {
        q->next = p->next->next;
      }
      p->level+=1; //inc level
      insert(p);
      merge(level + 1);
      return;
    }
    q = p;
    p = p->next;
  }
}

void free(void* node)
{
  printf("free\n");
  node_t* p = (node_t*)node;
  p-=1;
  if (!p) {
    return;
  }
  printf("merging\n");
  int level = p->level;
  printf("level %d\n", level);
  insert(p);
  merge(level);
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
  printf("alloc size:%d\n", alloc_size);
  node_t* p;
  node_t* node;
  int closest_free_level = -1;

  if (!heap) {
    init();
  }

  int level = get_level(alloc_size);
  fprintf(stderr, "level to get: %d\n", level);
  if (freelist[level] != NULL) {
    node = pop(level);
    fprintf(stderr, "Get directly\n");
  } else {
    fprintf(stderr, "Split på g\n");
    // closest_free_level = -1;
    for (size_t i = level + 1; i < 8; i++) {
      if(freelist[i] != NULL) {
        closest_free_level = i;
        fprintf(stderr, "Level to split %d\n", closest_free_level);
        break;
      }
    }
    if (!closest_free_level) {
      //Add on new memory
      fprintf(stderr, "NEED MORE MEMORY\n");
       p = (node_t*) sbrk(0);
       void* req = sbrk(INIT_SIZE);
       if (!req) {
         printf("out of memory!!\n");
         return NULL;
       }
       // create_node(p, INIT_SIZE);
       p->level = MAX_LEVEL;
       p->next = NULL;
       freelist[MAX_LEVEL] = p;
       closest_free_level = MAX_LEVEL;
    }
    node = split(closest_free_level, level);

  }
  // node_t* node = pop_list(freelist, level);
  // node->vacant = 0;
  // node_t* node = freelist[level];
  return (node+1);
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
