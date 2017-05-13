#ifndef __USER_H__
#define __USER_H__

#include <util/id.h>
#include <util/time.h>


typedef struct user {
  id id;
  time arrival;
} user;


#endif /* __USER_H__ */
