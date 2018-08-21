#include "malloc.h"
//include malloc.h for node_t

node_t* pop_list(node_t** freelist, int level);
node_t* add_last(node_t** freelist, int level, node_t* node);
node_t* remove_node(node_t** freelist, int level, node_t* node);
