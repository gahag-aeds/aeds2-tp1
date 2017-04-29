#ifndef __USER_H__
#define __USER_H__

#include <libaeds/memory.h>

#include <util/id.h>
#include <util/time.h>


typedef struct user {
  id id;
  time arrival;
} user;

extern user* new_user(allocator, id, time);
extern void delete_user(allocator, void*);


#endif /* __USER_H__ */
