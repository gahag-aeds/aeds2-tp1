#ifndef __TRAY_H__
#define __TRAY_H__

#include <libaeds/memory.h>

#include <util/id.h>


typedef struct tray {
  id id;
} tray;

extern tray* new_tray(allocator, id);
extern void delete_tray(allocator, void*);


#endif /* __TRAY_H__ */
