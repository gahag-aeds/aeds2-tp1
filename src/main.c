#include <stdio.h>
#include <stdlib.h>

#include <libaeds/array.h>
#include <libaeds/memory/allocator.h>
#include <libaeds/memory/vectorpool.h>
#include <libaeds/adt/queue.h>
#include <libaeds/adt/stack.h>

#include <config.h>
#include <foodservice.h>

#include <util/id.h>
#include <util/time.h>

#include <entity/user.h>


int main(int argc, char* argv[]) {
  allocator mallocator = std_allocator(abort);
  
  const time total_time = 4 * 60; // 4 hours.
  time total_user_time = 0;
  
  size_t served_users_count = 0;
  size_t trays_count = 0;
  
  idseed user_idseed = { .seed = 0 }; // Start with 0, to use the id as a total user count.
  void* phantom_tray = NULL + 1;  // Warning: do NOT dereference!
  
  
  config cfg; {
    int cfg_status = load_cfg(mallocator, argc, argv, &cfg);
    
    if (cfg_status != 0)
      return cfg_status;
  }
  
  
  allocator user_pool = new_vpool(
    mallocator,
    cfg.user_income * total_time,
    sizeof(user),
    abort
  );
  
  allocator llist_node_pool = new_vpool(
    mallocator,
    cfg.user_income * total_time,
    sizeof(listnode),
    abort
  );
  
  
  user* cashiers[cfg.cashiers_count]; {
    array_fill(cfg.cashiers_count, (void**) cashiers, NULL);
  }
  
  queue user_cashier_queues[cfg.cashiers_count]; {  // One queue for each cashier.
    foreach_ix (i, 0, cfg.cashiers_count)
      user_cashier_queues[i] = new_lqueue(llist_node_pool);
  }
  
  queue user_tray_queues[cfg.tray_stacks_count]; {  // One queue for each tray stack.
    foreach_ix (i, 0, cfg.tray_stacks_count)
      user_tray_queues[i] = new_lqueue(llist_node_pool);
  }
  
  stack tray_stacks[cfg.tray_stacks_count]; {
    foreach_ix (i, 0, cfg.tray_stacks_count)
      tray_stacks[i] = new_vstack(mallocator, cfg.tray_stack_max);
  }
  
  foodservice food_services[cfg.tray_stacks_count]; { // A food service for each tray stack.
    foreach_ix (i, 0, cfg.tray_stacks_count)
      food_services[i] = new_foodservice(mallocator, cfg.food_service_size);
  }
  
  
  
  // Loop for each chrono:
  for (time elapsed_time = 0; elapsed_time < total_time; elapsed_time++) {
    // First of all, if it is time, reload the tray stacks:
    if (elapsed_time % cfg.tray_reload_rate == 0)
      foreach_ix (i, 0, cfg.tray_stacks_count) // In each tray stack
        foreach_ix (_, 0, cfg.tray_reload_load) { // Reload the specified number of trays.
          if (stack_push(&tray_stacks[i], phantom_tray))
            trays_count++;
          else
            break;  // Just break if the tray stack is full.
        }
    
    user* usr = NULL; // Intermediary variable.
    
    // Users arrive in the restaurant:
    foreach_ix (i, 0, cfg.user_income) {
      usr = al_alloc(user_pool, 1, sizeof(user));
      *usr = (user) {
        .id = create_id(&user_idseed),
        .arrival = elapsed_time
      };
      
      // User enters the next cashier queue:
      enqueue(&user_cashier_queues[i % cfg.cashiers_count], usr);
    }
    
    // Cashiers:
    foreach_ix (i, 0, cfg.cashiers_count) {
      // User leaves the cashier, proceeding to the tray queue:
      usr = cashiers[i];
      
      // User leaves a cashier queue, entering the respective cashier:
      cashiers[i] = dequeue(&user_cashier_queues[i]);
      
      // User enters the next tray queue:
      enqueue(&user_tray_queues[i % cfg.tray_stacks_count], usr);
    }
    
    // Tray stacks, food services:
    foreach_ix (i, 0, cfg.tray_stacks_count) {
      // If there are trays available:
      if (!stack_empty(tray_stacks[i])) {
        // User leaves the tray queue, proceeding to the respective tray stack:
        usr = dequeue(&user_tray_queues[i]);
        
        // User, if any, grabs the tray:
        if (usr != NULL)
          stack_pop(&tray_stacks[i]); // No need to delete the tray. Using phantom trays.
      }
      else          // There were no trays available,
        usr = NULL; // so no user came from the tray stack to serve food.
      
      // User enters the respective food service, and the user at the last food bay leaves:
      usr = foodservice_shift(food_services[i], cfg.food_service_size, usr);
      
      // If an user left the food service, it ended the course:
      if (usr != NULL) {
        time user_time = elapsed_time - usr->arrival;
        
        total_user_time += user_time;
        served_users_count++;
        
        al_dealloc(user_pool, usr);
      }
    }
  }
  
  // Decimal minutes are irrelevant...
  time average_user_time = served_users_count == 0 ? 0
                                                   : total_user_time / served_users_count;
  
  
  printf("Total users: %lu\n", user_idseed.seed);
  printf("Total trays: %zu\n", trays_count);
  printf("Served users: %zu\n", served_users_count);
  printf("Total user time: %zuh %zum\n", total_user_time / 60, total_user_time % 60);
  printf("Average user time: %zuh %zum\n", average_user_time / 60, average_user_time % 60);
  
  
  
  // Free resources:
  foreach_ix (i, 0, cfg.cashiers_count)
    // Delete user_cashier_queues:
    delete_queue(&user_cashier_queues[i], NULL, null_allocator());
  
  foreach_ix (i, 0, cfg.tray_stacks_count) {
    // Delete user_tray_queues:
    delete_queue(&user_tray_queues[i], NULL, null_allocator());
    
    // Delete tray_stacks
    delete_stack(&tray_stacks[i], NULL, null_allocator());
    
    // Delete food_service:
    delete_foodservice(
      mallocator,
      food_services[i],
      cfg.food_service_size,
      NULL,
      null_allocator()
    );
  }
  
  delete_vpool(&user_pool);
  delete_vpool(&llist_node_pool);
  
  
  
  return 0;
}
