#ifndef __FOOD_SERVICE_H__
#define __FOOD_SERVICE_H__

#include <libaeds/memory.h>
#include <entity/user.h>


typedef user** foodservice;

foodservice new_foodservice(allocator, size_t);
void delete_foodservice(allocator, allocator, foodservice, size_t);

user* foodservice_shift(foodservice, size_t, user*);


#endif /* __FOOD_SERVICE_H__ */
