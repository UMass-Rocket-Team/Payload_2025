#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdint.h>

#include <pico/stdlib.h>

typedef uint8_t task_priority_t;
typedef uint64_t task_time_t;

#define TASK_IDLE_PRIORITY (task_priority_t)0

typedef struct {

    // task name
    char* name;

    // priority number, higher number = higher priority
    task_priority_t priority;

    // task callback pointer
    void (*task_cb)(void*);

    // arguments to pass to task callback
    void* args;

    // time that the task must run next, uS
    task_time_t next_run;

} task_t;

#define task_init_default(nm, cb) (task_t){.name=nm, .priority=TASK_IDLE_PRIORITY, .task_cb=cb, .args=NULL, .next_run=(task_time_t)time_us_64()}

int scheduler_init();
int scheduler_run();

int scheduler_add_task(task_t* task);

#endif