#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <libaeds/memory.h>

#include <util/time.h>


typedef struct config {
  size_t user_income
       , food_service_size
       , tray_stack_max
       , tray_reload_load;
  
  time tray_reload_rate;
} config;


extern int load_cfg(allocator, int argc, char* argv[], config* cfg);

#endif /* __CONFIG_H__ */
