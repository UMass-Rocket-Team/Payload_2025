#include <stdio.h>
#include "pico/stdlib.h"

#include "schedule.h"

void task_periodic_core0_cb(void* args);

task_t task_periodic_core0 = {
    .args = NULL,
    .name = "PERIODIC0",
    .priority = TASK_IDLE_PRIORITY + 1,
    .next_run = 0,
    .task_cb = task_periodic_core0_cb
};

void task_periodic_core0_cb(void* args) {
    printf("Hello, world!\n");
    task_periodic_core0.next_run = time_us_64() + 1000000;
}

int main()
{
    stdio_init_all();

    // wait for user input
    getchar();

    // Initialize the scheduler
    int ret = scheduler_init();
    if ( ret != 0 ) {
        printf("Failed to initialize scheduler\n");
    }

    // Add the periodic task to the scheduler
    ret = scheduler_add_task(&task_periodic_core0);
    if ( ret != 0 ) {
        printf("Failed to add task_periodic_core0\n");
    }
    
    // Run the scheduler
    while (1) {
        scheduler_run();
    }

}
