#include "malloc.h"
#include "malloc.c"





int main(int argc, char const *argv[]) {
  printf("size: %zu\n", sizeof(int) + sizeof(node_t));
  int* a = malloc(2*sizeof(int));
  // printf("tester:%zu\n", a[]);
  a[0] = 42;
  a[1] = 15;
  printf("a val: %d address:%zu, pointer val: %zu\n", *a, &a, a);
  // free(a);
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
