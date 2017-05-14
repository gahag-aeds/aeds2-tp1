#include "foodservice.h"

#include <libaeds/array.h>


// O(n)
foodservice new_foodservice(allocator allocator, size_t size) {
  foodservice fs = al_alloc(allocator, size, sizeof(user*));
  
  array_fill(size, (void**) fs, NULL); // O(n)
  
  return fs;
}

// O(1) if delete is NULL.
// O(n) otherwise.
void delete_foodservice(
  allocator alloc,
  foodservice fs,
  size_t size,
  void (*delete)(allocator, void*),
  allocator user_alloc
) {
  if (delete != NULL)
    foreach_ix (i, 0, size) // O(n)
      if (fs[i] != NULL)  // O(1)
        delete(user_alloc, fs[i]);
  
  al_dealloc(alloc, fs);
}


// O(n)
user* foodservice_shift(foodservice food_service, size_t size, user* usr) {
  user* final = food_service[size - 1];
  
  for (size_t i = size - 1; i != 0; i--)  // O(n - 1)
    food_service[i] = food_service[i - 1]; // O(1)
  
  food_service[0] = usr;
  
  return final;
}
