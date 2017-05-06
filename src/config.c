#include "config.h"

#include <stdio.h>

#include <libaeds/args.h>
#include <libaeds/console.h>


static int stdin_args(
  __attribute__((unused)) const char* _,
  void* parameter
) {
  config* cfg = parameter;
  
  if (!prompt("User income (users/min): ",             "%zu", &cfg->user_income)       ||
      !prompt("Food service size (bays): ",            "%zu", &cfg->food_service_size) ||
      !prompt("Tray stack maximum capacity (trays): ", "%zu", &cfg->tray_stack_max)    ||
      !prompt("Tray reload load (trays): ",            "%zu", &cfg->tray_reload_load)  ||
      !prompt("Tray reload rate (minutes): ",          "%zu", &cfg->tray_reload_rate))
    return -1;
  
  return 0;
}

static int file_args(
  const char* file_name,
  void* parameter
) {
  config* cfg = parameter;
  
  FILE* cfg_file = fopen(file_name, "r");
  
  if (cfg_file == NULL)
    return -1;
  
  if (fscanf(cfg_file, "%zu", &cfg->user_income)       != 1 ||
      fscanf(cfg_file, "%zu", &cfg->food_service_size) != 1 ||
      fscanf(cfg_file, "%zu", &cfg->tray_stack_max)    != 1 ||
      fscanf(cfg_file, "%zu", &cfg->tray_reload_load)  != 1 ||
      fscanf(cfg_file, "%zu", &cfg->tray_reload_rate)  != 1)
      return -2;
  
  return 0;
}


int load_cfg(allocator allocator, int argc, char* argv[], config* cfg) {
  argv_handler args_handler;
  
  bool parsed = handle_args(
    allocator, &args_handler,
    
    argc, argv,
    
    2,
    (argv_handler[]) {
      new_argv_handler(allocator, 1, cfg, (arg_handler[]){ stdin_args }),
      new_argv_handler(allocator, 2, cfg, (arg_handler[]){ NULL, file_args })
    }
  );
  
  if (!parsed) {
    fputs("Invalid number of arguments!\n", stderr);
    return -1;
  }
  
  // No file specified, cfg read from stdin.
  if (args_handler.argv_size == 1 && args_handler.results[0] < 0) {
    fputs("Invalid option!\n", stderr);
    return -2;
  }
  // File specified, cfg read from file:
  if (args_handler.argv_size == 2 && args_handler.results[1] < 0)
    switch(args_handler.results[1]) {
      case -1:
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        return -3;
      
      case -2:
        fprintf(stderr, "Failed to parse file %s\n", argv[1]);
        return -4;
    }

  
  delete_argv_handler(allocator, &args_handler);
  
  return 0;
}
