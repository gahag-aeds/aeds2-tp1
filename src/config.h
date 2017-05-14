#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>

#include <libaeds/memory/allocator.h>

#include <util/time.h>


// The configuration parameters of the restaurant.
typedef struct config {
  size_t user_income;
  
  size_t cashiers_count;
    
  size_t tray_stacks_count
       , tray_stack_max
       , tray_reload_load;
  
  time tray_reload_rate;
        
  size_t food_service_size;
} config;


// Load the program config.
// If a file name is supplied as the only argument to the program,
// then it attempts to load the config from that file.
// Otherwise, attempts to read the config from stdin.
// It uses an allocator to supply the handle_args function of args.h in libaeds.
//
// A config file must have the following format:
// One field at the beginning of each line. Any other text might succeed each field.
// All fields are numeric. An early EOF, or a fscanf failure results in parse failure.
// Fields are parsed in the following order:
// * user income (users/min)
// * cashiers count
// * tray stacks count
// * tray stack maximum capacity (trays)
// * tray reload load (trays)
// * tray reload rate (mins/reload)
// * food service size (bays)
// 
// Returns one of the following status code:
// * 0  : loaded config with success.
// * -1 : more than one argument was supplied to the program.
// * -2 : an invalid value to a config parameter was supplied in stdin.
// * -3 : failed to open the specified config file.
// * -4 : failed to parse the specified config file.
// 
// Complexity: O(1)
extern int load_cfg(allocator, int argc, char* argv[static argc], config* cfg);

// Pretty print a config to stdout.
// Complexity: O(1)
extern bool print_cfg(config cfg);


#endif /* __CONFIG_H__ */
