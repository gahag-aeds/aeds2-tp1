#include "user.h"


user* new_user(allocator allocator, id id, time arrival) {
  user* user = al_alloc(allocator, 1, sizeof(*user));
  
  user->id = id;
  user->arrival = arrival;
  
  return user;
}

void delete_user(allocator allocator, void* user) {
  al_dealloc(allocator, user);
}
