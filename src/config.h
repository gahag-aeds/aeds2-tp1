#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>

#include <libaeds/memory/allocator.h>

#include <util/time.h>


typedef struct config {
  size_t user_income;
  
  size_t cashiers_count;
    
  size_t tray_stacks_count
       , tray_stack_max
       , tray_reload_load;
  
  time tray_reload_rate;
        
  size_t food_service_size;
} config;


extern int load_cfg(allocator, int argc, char* argv[static argc], config* cfg);

extern bool print_cfg(config cfg);


#endif /* __CONFIG_H__ */
