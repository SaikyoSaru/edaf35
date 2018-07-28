#include "malloc.h"
#include "malloc.c"




int main(int argc, char const *argv[]) {

  int* a = malloc(sizeof(int));
  // printf("tester:%zu\n", a[]);
  a[0] = 42;
  a[1] = 15;
  printf("a val: %d address:%zu, pointer val: %zu\n", *a, &a, a);
  a += 1;
  printf("a val: %d address:%zu, pointer val: %zu\n", *a, &a, a);
  int* b = calloc(1, sizeof(int));
  printf("b val: %d address:%zu, pointer val: %zu\n", *b, &b, b);
  free(b);
  int* b2 = calloc(1, sizeof(int));
  printf("b val: %d address:%zu, pointer val: %zu\n", *b2, &b2, b2);

  free(a);



}
