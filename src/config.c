#include "config.h"

#include <assert.h>
#include <stdio.h>

#include <libaeds/args.h>
#include <libaeds/io/console.h>
#include <libaeds/io/file.h>


// O(1)
static int stdin_args(
  __attribute__((unused)) const char* _,
  void* parameter
) {
  assert(parameter != NULL);
  
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

// O(1)
static int file_args(
  const char* file_name,
  void* parameter
) {
  assert(parameter != NULL);
  
  config* cfg = parameter;
  
  FILE* cfg_file = fopen(file_name, "r");
  
  if (cfg_file == NULL)
    return -1;
  
  if (fscanf(cfg_file, "%zu", &cfg->user_income)       != 1 || !file_skip_line(cfg_file) ||
      fscanf(cfg_file, "%zu", &cfg->cashiers_count)    != 1 || !file_skip_line(cfg_file) ||
      fscanf(cfg_file, "%zu", &cfg->tray_stacks_count) != 1 || !file_skip_line(cfg_file) ||
      fscanf(cfg_file, "%zu", &cfg->tray_stack_max)    != 1 || !file_skip_line(cfg_file) ||
      fscanf(cfg_file, "%zu", &cfg->tray_reload_load)  != 1 || !file_skip_line(cfg_file) ||
      fscanf(cfg_file, "%zu", &cfg->tray_reload_rate)  != 1 || !file_skip_line(cfg_file) ||
      fscanf(cfg_file, "%zu", &cfg->food_service_size) != 1 )
    return -2;
  
  fclose(cfg_file);
  
  return 0;
}


// O(1)
int load_cfg(allocator allocator, int argc, char* argv[static argc], config* cfg) {
  assert(argv != NULL);
  assert(cfg != NULL);
  
  argvresults results;
  
  bool parsed = handle_args(  // O(1 + 1) worst.
    allocator, &results,
    
    argc, argv,
    
    2,
    (argvhandler[]) {
      new_argvhandler(allocator, 1, cfg, (arghandler[]) { stdin_args }), // O(1)
      new_argvhandler(allocator, 2, cfg, (arghandler[]) { NULL, file_args }) // O(1)
    }
  );
  
  if (!parsed) {
    fputs("Invalid number of arguments!\n", stderr);
    delete_argvresults(&results); // O(1)
    return -1;
  }
  
  // No file specified, cfg read from stdin.
  if (results.argv_size == 1 && results.data[0] < 0) {
    fputs("Invalid option!\n", stderr);
    delete_argvresults(&results); // O(1)
    return -2;
  }
  // File specified, cfg read from file:
  if (results.argv_size == 2 && results.data[1] < 0)
    switch(results.data[1]) {
      case -1:
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        delete_argvresults(&results); // O(1)
        return -3;
      
      case -2:
        fprintf(stderr, "Failed to parse file %s\n", argv[1]);
        delete_argvresults(&results); // O(1)
        return -4;
    }
  
  
  delete_argvresults(&results); // O(1)
  
  return 0;
}


// O(1)
bool print_cfg(config cfg) {
  return printf("User income (users/min): %zu\n",             cfg.user_income)       > 0
      && printf("Number of cashiers (count): %zu\n",          cfg.cashiers_count)    > 0
      && printf("Number of tray stacks (count): %zu\n",       cfg.tray_stacks_count) > 0
      && printf("Tray stack maximum capacity (trays): %zu\n", cfg.tray_stack_max)    > 0
      && printf("Tray reload load (trays): %zu\n",            cfg.tray_reload_load)  > 0
      && printf("Tray reload rate (minutes): %zu\n",          cfg.tray_reload_rate)  > 0
      && printf("Food service size (bays): %zu\n",            cfg.food_service_size) > 0;
}
