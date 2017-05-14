#ifndef __FOOD_SERVICE_H__
#define __FOOD_SERVICE_H__

#include <libaeds/memory/allocator.h>

#include <entity/user.h>


// The food service is an array of food bays.
// Each bay may contain an user self-servicing.
typedef user** foodservice;


// Create an food service of the specified size, using the specified allocator.
// The resultant foodservice must be deleted with delete_foodservice when no longer used.
// Complexity: O(n)
foodservice new_foodservice(allocator, size_t);

// Delete an foodservice of the specified size, using the specified allocator.
// The foodservice must have been created with new_foodservice.
// If delete is not NULL, the `user_alloc` and each user in the food service is
// supplied to the delete function.
// Complexity:
// O(1) if delete is NULL.
// O(n) otherwise.
void delete_foodservice(
  allocator,
  foodservice,
  size_t,
  void (*delete)(allocator, void*),
  allocator user_alloc
);

// Shift the users in a food service.
// Each user goes to the next bay. The supplied user goes into the first bay.
// Returns the user that was at the last bay.
// Complexity: O(n) where n is the size of the foodservice.
user* foodservice_shift(foodservice, size_t, user*);


#endif /* __FOOD_SERVICE_H__ */
