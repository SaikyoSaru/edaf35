#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

typedef struct node_t node_t;
#define INIT_SIZE 32

void* root = NULL;

struct node_t {
  node_t* left;
  node_t* right;
  size_t size;
  int vacant;
};

void* init(size_t size)
{
  printf("Init\n");
  node_t* p = sbrk(0);
  void* req = sbrk(size + sizeof(node_t));
  if (!req) {
    return NULL;
  }
  p->left = NULL;
  p->right = NULL;
  p->vacant = 1;
  p->size = size;
  printf("init done\n");
  return p;
}

node_t* create_node(node_t* node, size_t size)
{
  printf ("create node\n");
  node->left = NULL;
  node->right = NULL;
  node->vacant = 1;
  node->size = size;
  return node;
}

node_t* split(node_t* node, size_t size)
{
  printf("split\n");
  if (node->size / 2 < size) {
    printf ("split done\n");
    return node;
  } else {
    printf ("left\n");
    node->left = create_node(node, (node->size)/2);
    if (!node->left ) {
      printf("aiuds\n");
    }
    printf("right\n");
    node->right = create_node(node + (node->size)/2, (node->size)/2);
    return split(node->left, size);
  }
}

node_t* merge()
{

}

node_t* find_free_spot(node_t* node, size_t size)
{
  printf("find free spot\n");
  if (node->vacant && node->size >= size) {
    return node;
  }
  if (!node->left && node->left->size >= size && node->left->vacant) {
    return find_free_spot(node->left, size);
  } else if (!node->right && node->right->vacant){
    return find_free_spot(node->right, size);
  }
  return NULL;
}

node_t* create_block(node_t* node, size_t size)
{
  printf("create block\n");
  node_t* p = find_free_spot(node, size);
  if (!p) {
    return NULL;
  }
  p = split(p, size);
  p->vacant = 0;
  return p;
}

void free(void* node)
{
  if (!node) {
    return;
  }
  merge(node);
  ((node_t*)node)->vacant = 1;
}

void* malloc(size_t size)
{
  printf("malloc\n");
  node_t* p;
  if (!root) {
    root = init(INIT_SIZE);
  } else {
    p = create_block(root, size);
    if (!p) {
      printf ("NULL");
      return NULL;
    }
    p->vacant = 1;
  }
  return p+1;
}


void* calloc(size_t size)
{
  node_t* p = malloc(size);
  if (p != NULL) {
    memset(p, 0, size);
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

int main(){

  int* test = calloc(sizeof(int));
  printf("test 1 malloc: %d at address: %d\n", *test, test);
  printf("done1\n");
  int* test2 = calloc(sizeof(int));
  printf("test 2 malloc: %d at address: %d\n", *test2, test2);
  printf("done2\n");
  int* test3 = calloc(sizeof(int));
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
