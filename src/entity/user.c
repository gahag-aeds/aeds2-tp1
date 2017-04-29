#include <stdlib.h>

#include "user.h"


user* new_user(allocator allocator, id id, time arrival) {
  user* user = alloc(allocator, sizeof(*user));
  
  user->id = id;
  user->arrival = arrival;
  
  return user;
}

void delete_user(allocator allocator, void* user) {
  dealloc(allocator, user);
}
