#include <stdlib.h>

#include "tray.h"


tray* new_tray(allocator allocator, id id) {
  tray* tray = alloc(allocator, sizeof(*tray));
  
  tray->id = id;
  
  return tray;
}

void delete_tray(allocator allocator, void* ptr) {
  dealloc(allocator, ptr);
}
