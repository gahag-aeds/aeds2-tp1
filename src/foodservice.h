#ifndef __FOOD_SERVICE_H__
#define __FOOD_SERVICE_H__

#include <libaeds/memory/allocator.h>

#include <entity/user.h>


typedef user** foodservice;

foodservice new_foodservice(allocator, size_t);
void delete_foodservice(
  allocator,
  foodservice,
  size_t,
  void (*delete)(allocator, void*),
  allocator user_alloc
);

user* foodservice_shift(foodservice, size_t, user*);


#endif /* __FOOD_SERVICE_H__ */
