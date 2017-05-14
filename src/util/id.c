#include "id.h"


// O(1)
id create_id(idseed* is) {
  return ++is->seed;
}
