#include "id.h"

#include <assert.h>
#include <stddef.h>


// O(1)
id create_id(idseed* is) {
  assert(is != NULL);
  
  return ++is->seed;
}
