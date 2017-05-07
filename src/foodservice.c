#include "foodservice.h"


foodservice new_foodservice(allocator allocator, size_t size) {
  foodservice fs = al_alloc(allocator, size, sizeof(user*));
  
  for (size_t i = 0; i < size; i++)
    fs[i] = NULL;
  
  return fs;
}

void delete_foodservice(
  allocator alloc,
  allocator user_alloc,
  foodservice fs,
  size_t size
) {
  for (size_t i = 0; i < size; i++)
    if (fs[i] != NULL)
      delete_user(user_alloc, fs[i]);
  
  al_dealloc(alloc, fs);
}


user* foodservice_shift(foodservice food_service, size_t size, user* usr) {
  for (size_t i = 0; i < size; i++) {
    user* aux = food_service[i];
    
    food_service[i] = usr;
    usr = aux;
  }
  
  return usr;
}
