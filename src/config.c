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
      !prompt("Number of cashiers (count): ",          "%zu", &cfg->cashiers_count)    ||
      !prompt("Number of tray stacks (count): ",       "%zu", &cfg->tray_stacks_count) ||
      !prompt("Tray stack maximum capacity (trays): ", "%zu", &cfg->tray_stack_max)    ||
      !prompt("Tray reload load (trays): ",            "%zu", &cfg->tray_reload_load)  ||
      !prompt("Tray reload rate (minutes): ",          "%zu", &cfg->tray_reload_rate)  ||
      !prompt("Food service size (bays): ",            "%zu", &cfg->food_service_size))
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
      fscanf(cfg_file, "%zu", &cfg->cashiers_count)    != 1 ||
      fscanf(cfg_file, "%zu", &cfg->tray_stacks_count) != 1 ||
      fscanf(cfg_file, "%zu", &cfg->tray_stack_max)    != 1 ||
      fscanf(cfg_file, "%zu", &cfg->tray_reload_load)  != 1 ||
      fscanf(cfg_file, "%zu", &cfg->tray_reload_rate)  != 1 ||
      fscanf(cfg_file, "%zu", &cfg->food_service_size) != 1)
      return -2;
  
  fclose(cfg_file);
  
  return 0;
}


int load_cfg(allocator allocator, int argc, char* argv[], config* cfg) {
  argvresults results;
  
  bool parsed = handle_args(
    allocator, &results,
    
    argc, argv,
    
    2,
    (argvhandler[]) {
      new_argvhandler(allocator, 1, cfg, (arg_handler[]){ stdin_args }),
      new_argvhandler(allocator, 2, cfg, (arg_handler[]){ NULL, file_args })
    }
  );
  
  if (!parsed) {
    fputs("Invalid number of arguments!\n", stderr);
    return -1;
  }
  
  // No file specified, cfg read from stdin.
  if (results.argv_size == 1 && results.data[0] < 0) {
    fputs("Invalid option!\n", stderr);
    return -2;
  }
  // File specified, cfg read from file:
  if (results.argv_size == 2 && results.data[1] < 0)
    switch(results.data[1]) {
      case -1:
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        return -3;
      
      case -2:
        fprintf(stderr, "Failed to parse file %s\n", argv[1]);
        return -4;
    }
  
  
  delete_argvresults(allocator, &results);
  
  return 0;
}


bool print_cfg(config cfg) {
  return printf("User income (users/min): %zu\n",             cfg.user_income)       > 0
      && printf("Number of cashiers (count): %zu\n",          cfg.cashiers_count)    > 0
      && printf("Number of tray stacks (count): %zu\n",       cfg.tray_stacks_count) > 0
      && printf("Tray stack maximum capacity (trays): %zu\n", cfg.tray_stack_max)    > 0
      && printf("Tray reload load (trays): %zu\n",            cfg.tray_reload_load)  > 0
      && printf("Tray reload rate (minutes): %zu\n",          cfg.tray_reload_rate)  > 0
      && printf("Food service size (bays): %zu\n",            cfg.food_service_size) > 0;
}
