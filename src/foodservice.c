#include "foodservice.h"

#include <libaeds/array.h>


foodservice new_foodservice(allocator allocator, size_t size) {
  foodservice fs = al_alloc(allocator, size, sizeof(user*));
  
  array_fill(size, (void**) fs, NULL);
  
  return fs;
}

void delete_foodservice(
  allocator alloc,
  foodservice fs,
  size_t size,
  void (*delete)(allocator, void*),
  allocator user_alloc
) {
  if (delete != NULL)
    for (size_t i = 0; i < size; i++)
      if (fs[i] != NULL)
        delete(user_alloc, fs[i]);
  
  al_dealloc(alloc, fs);
}


user* foodservice_shift(foodservice food_service, size_t size, user* usr) {
  user* final = food_service[size - 1];
  
  for (size_t i = size - 1; i != 0; i--)
    food_service[i] = food_service[i - 1];
  
  food_service[0] = usr;
  
  return final;
}
