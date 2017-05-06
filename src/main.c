#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libaeds/args.h>
#include <libaeds/console.h>
#include <libaeds/memory.h>
#include <libaeds/adt/queue.h>
#include <libaeds/adt/stack.h>

#include <util/id.h>
#include <util/time.h>

#include <entity/user.h>
#include <entity/tray.h>

#include <config.h>
#include <foodservice.h>


// Parameters:
//
// number of user token queues
// number of user stack queues
// number of tray stacks


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
  
  config cfg;
  
  const time total_time = 4 * 60; // 4 hours.
  time total_user_time = 0;
  size_t served_users_count = 0;
  
  idseed user_idseed = { 0 }; // Start with 0, to use the id as a total user count.
  idseed tray_idseed = { 0 }; // Start with 0, to use the id as a total tray count.
  
  user* cashier_user = NULL;  // The user at the cashier.
  
  queue user_cashier_queue = new_lqueue(mallocator);
  queue user_tray_queue = new_lqueue(mallocator);
  
  stack tray_stack;
  
  foodservice food_service;
  
  
  int cfg_status = load_cfg(mallocator, argc, argv, &cfg);
  
  if (cfg_status != 0)
    return cfg_status;
  
  
  tray_stack = new_vstack(mallocator, cfg.tray_stack_max);
  
  food_service = new_foodservice(mallocator, cfg.food_service_size);
  
  
  for (time elapsed_time = 0; elapsed_time < total_time; elapsed_time++) {
    if (elapsed_time % cfg.tray_reload_rate == 0)
      reload_tray_stack(mallocator, &tray_idseed, &tray_stack, cfg.tray_reload_load);
    
    user* usr = NULL;
    
    for (size_t i = 0; i < cfg.user_income; i++) {
      usr = new_user(mallocator, create_id(&user_idseed), elapsed_time); // User arrives.
      
      enqueue(&user_cashier_queue, usr); // User enters the cashier queue.
    }
    
    usr = cashier_user; // User leaves the cashier, proceeding to the tray queue.
    cashier_user = dequeue(&user_cashier_queue); // User leaves the cashier queue,
                                                 // entering the cashier.
    
    enqueue(&user_tray_queue, usr); // User enters the tray queue.
    
    if (!stack_empty(tray_stack)) {
      usr = dequeue(&user_tray_queue); // User leaves the tray queue.
      
      if (usr != NULL)  // User grabs the tray.
        delete_tray(mallocator, stack_pop(&tray_stack));
    }
    else
      usr = NULL; // There were no trays available,
                  // so no user came from the tray stack to serve food.
    
    // User enters the food service, and the user at the last food bay proceeds to eat.
    usr = foodservice_shift(food_service, cfg.food_service_size, usr);
    
    if (usr != NULL) {
      time user_time = elapsed_time - usr->arrival;
      total_user_time += user_time;
      served_users_count++;
      
      // printf("User #%lu time: %zuh %zum\n", usr->id, user_time / 60, user_time % 60);
      
      delete_user(mallocator, usr);
    }
  }
  
  
  // Decimal minutes are irrelevant...
  time average_user_time = total_user_time / served_users_count;
  
  printf("Total users: %lu\n", user_idseed.seed);
  printf("Total trays: %lu\n", tray_idseed.seed);
  printf("Served users: %zu\n", served_users_count);
  printf("Total user time: %zuh %zum\n", total_user_time / 60, total_user_time % 60);
  printf("Average user time: %zuh %zum\n", average_user_time / 60, average_user_time % 60);
  
  
  delete_user(mallocator, cashier_user);
  delete_foodservice(mallocator, mallocator, food_service, cfg.food_service_size);
  
  delete_queue(&user_cashier_queue, delete_user, mallocator);
  delete_queue(&user_tray_queue, delete_user, mallocator);
  delete_stack(&tray_stack, delete_tray, mallocator);
  
  
  return 0;
}
