#include <stdio.h>
#include <stdlib.h>

#include <libaeds/args.h>
#include <libaeds/array.h>
#include <libaeds/memory.h>
#include <libaeds/adt/queue.h>
#include <libaeds/adt/stack.h>

#include <config.h>
#include <foodservice.h>

#include <util/id.h>
#include <util/time.h>

#include <entity/user.h>
#include <entity/tray.h>


void reload_tray_stack(
  allocator allocator,
  idseed* seed,
  stack* tray_stack,
  size_t load
) {
  idseed prev_seed;
  
  for (; load > 0; load--) {
    prev_seed = *seed;
    
    tray* tray = new_tray(allocator, create_id(seed));
    
    if (!stack_push(tray_stack, tray)) {
      delete_tray(allocator, tray);
      
      *seed = prev_seed; // Reset the seed, since the tray was of no use.
      
      break;
    }
  }
}


int main(int argc, char *argv[]) {
  const allocator mallocator = std_allocator(abort);
  
  const time total_time = 4 * 60; // 4 hours.
  time total_user_time = 0;
  size_t served_users_count = 0;
  
  idseed user_idseed = { .seed = 0 }; // Start with 0, to use the id as a total user count.
  idseed tray_idseed = { .seed = 0 }; // Start with 0, to use the id as a total tray count.
  
  
  config cfg;
  int cfg_status = load_cfg(mallocator, argc, argv, &cfg);
  
  if (cfg_status != 0)
    return cfg_status;
  
  
  user* cashiers[cfg.cashiers_count];
  array_fill(cfg.cashiers_count, (void**) cashiers, NULL);
  
  queue user_cashier_queues[cfg.cashiers_count]; // One queue for each cashier.
  foreach (i, 0, cfg.cashiers_count)
    user_cashier_queues[i] = new_lqueue(mallocator);
  
  queue user_tray_queues[cfg.tray_stacks_count]; // One queue for each tray stack.
  foreach (i, 0, cfg.tray_stacks_count)
    user_tray_queues[i] = new_lqueue(mallocator);
  
  stack tray_stacks[cfg.tray_stacks_count];
  foreach (i, 0, cfg.tray_stacks_count)
    tray_stacks[i] = new_vstack(mallocator, cfg.tray_stack_max);
  
  foodservice food_services[cfg.tray_stacks_count]; // One food service for each tray stack.
  foreach (i, 0, cfg.tray_stacks_count)
    food_services[i] = new_foodservice(mallocator, cfg.food_service_size);
  
  
  for (time elapsed_time = 0; elapsed_time < total_time; elapsed_time++) {
    // First of all, reload the tray stacks:
    if (elapsed_time % cfg.tray_reload_rate == 0)
      foreach (i, 0, cfg.tray_stacks_count)
        reload_tray_stack(mallocator, &tray_idseed, &tray_stacks[i], cfg.tray_reload_load);
    
    user* usr = NULL; // Intermediary variable.
    
    foreach (i, 0, cfg.user_income) {
      // User arrives the course:
      usr = new_user(mallocator, create_id(&user_idseed), elapsed_time);
      
      // User enters the next cashier queue:
      enqueue(&user_cashier_queues[i % cfg.cashiers_count], usr);
    }
    
    
    foreach (i, 0, cfg.cashiers_count) {
      // User leaves the cashier, proceeding to the tray queue:
      usr = cashiers[i];
      
      // User leaves a cashier queue, entering the respective cashier:
      cashiers[i] = dequeue(&user_cashier_queues[i]);
      
      // User enters the next tray queue:
      enqueue(&user_tray_queues[i % cfg.tray_stacks_count], usr);
    }
    
    
    foreach (i, 0, cfg.tray_stacks_count) {
      if (!stack_empty(tray_stacks[i])) {
        // User leaves the tray queue, proceeding to the respective tray stack:
        usr = dequeue(&user_tray_queues[i]);
        
        // User grabs the tray:
        if (usr != NULL)
          delete_tray(mallocator, stack_pop(&tray_stacks[i]));
      }
      else
        // There were no trays available, so no user came from the tray stack to serve food:
        usr = NULL;
      
      // User enters the respective food service,
      // and the user at the last food bay proceeds to eat:
      usr = foodservice_shift(food_services[i], cfg.food_service_size, usr);
      
      // User ended the course, proceeding to eat.
      if (usr != NULL) {
        time user_time = elapsed_time - usr->arrival;
        total_user_time += user_time;
        served_users_count++;
        
        delete_user(mallocator, usr);
      }
    }
  }
  
  
  // Decimal minutes are irrelevant...
  time average_user_time = served_users_count == 0 ? 0
                                                   : total_user_time / served_users_count;
  
  printf("Total users: %lu\n", user_idseed.seed);
  printf("Total trays: %lu\n", tray_idseed.seed);
  printf("Served users: %zu\n", served_users_count);
  printf("Total user time: %zuh %zum\n", total_user_time / 60, total_user_time % 60);
  printf("Average user time: %zuh %zum\n", average_user_time / 60, average_user_time % 60);
  
  
  foreach (i, 0, cfg.cashiers_count) {
    // Delete cashiers:
    delete_user(mallocator, cashiers[i]);
    
    // Delete user_cashier_queues:
    delete_queue(&user_cashier_queues[i], delete_user, mallocator);
  }
  
  foreach (i, 0, cfg.tray_stacks_count) {
    // Delete user_tray_queues:
    delete_queue(&user_tray_queues[i], delete_user, mallocator);
    
    // Delete tray_stacks
    delete_stack(&tray_stacks[i], delete_tray, mallocator);
    
    // Delete food_service:
    delete_foodservice(mallocator, mallocator, food_services[i], cfg.food_service_size);
  }
  
  
  return 0;
}
