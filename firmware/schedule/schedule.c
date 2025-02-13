#include "schedule.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "pico/sem.h"

#ifdef SCHEDULER_DEBUG
#include <stdio.h>
#define SCHEDULER_DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define SCHEDULER_DEBUG_PRINT(...)
#endif

#ifndef SCHEDULER_CORE0_TASK_CNT
#error "SCHEDULER_CORE0_TASK_CNT must be defined"
#else
task_t* _scheduler_core0_task_list[SCHEDULER_CORE0_TASK_CNT];
#endif

#ifndef SCHEDULER_CORE1_TASK_CNT
#error "SCHEDULER_CORE1_TASK_CNT must be defined"
#else
task_t* _scheduler_core1_task_list[SCHEDULER_CORE1_TASK_CNT];
#endif

typedef struct {

    task_t** tasks;

    size_t task_cnt;

} scheduler_t;

static scheduler_t scheduler_core0 = {
    .tasks = _scheduler_core0_task_list,
    .task_cnt = SCHEDULER_CORE0_TASK_CNT
};

static scheduler_t scheduler_core1 = {
    .tasks = _scheduler_core1_task_list,
    .task_cnt = SCHEDULER_CORE1_TASK_CNT
};

static inline scheduler_t* get_scheduler() {
    return (get_core_num() == 0) ? &scheduler_core0 : &scheduler_core1;
}

int scheduler_init() {
    SCHEDULER_DEBUG_PRINT("Initializing scheduler...\n");
    return 0;
}

int scheduler_add_task(task_t* task) {

    SCHEDULER_DEBUG_PRINT("Adding task %s\n", task->name);

    scheduler_t* current_scheduler = get_scheduler();

    // find the first available slot in the task list
    for (int i = 0; i < current_scheduler->task_cnt; i++) {
        if (current_scheduler->tasks[i] == NULL) {
            current_scheduler->tasks[i] = task;
            return 0;
        }
    }

    return -1;

}

int scheduler_run() {

    scheduler_t* current_scheduler = get_scheduler();

    task_t* highest_priority_task = NULL;
    int highest_priority = -1;
    uint64_t highest_next_run = UINT64_MAX;

    for (int i = 0; i < current_scheduler->task_cnt; i++) {
        task_t* task = current_scheduler->tasks[i];
        if (task != NULL && (task->next_run < time_us_64())) {
            if (task->priority > highest_priority || 
               (task->priority == highest_priority && task->next_run < highest_next_run)) {
                highest_priority_task = task;
                highest_priority = task->priority;
                highest_next_run = task->next_run;
            }
        }
    }

    if (highest_priority_task != NULL && highest_priority_task->task_cb != NULL) {
        highest_priority_task->task_cb(highest_priority_task->args);
    }

    return 0;

}