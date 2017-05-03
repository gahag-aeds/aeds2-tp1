#include <stdlib.h>

#include "tray.h"


tray* new_tray(allocator allocator, id id) {
  tray* tray = al_alloc(allocator, 1, sizeof(*tray));
  
  tray->id = id;
  
  return tray;
}

void delete_tray(allocator allocator, void* ptr) {
  al_dealloc(allocator, ptr);
}
