#include <stdio.h>
#include "memory.h"

typedef struct list_t list_t;

struct list_t {
  void* first;
};


void* malloc(size_t* size)
{
  printf("DUUUUDE");
}
