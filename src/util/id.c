#include "id.h"


id create_id(idseed* is) {
  return ++is->seed;
}
