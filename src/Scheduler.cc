#include <vector>
#include <utility>

#include "Scheduler.hh"
#include "Task.hh"
#include <Arduino.h>
#include "clean_arduino_macros.hh"

std::vector<Task> tasks;

uint32_t register_task(Task &&task)
{
    uint32_t pid = tasks.size();
    tasks.push_back(std::move(task));
    return pid;
}

void enable_task(uint32_t pid)
{
    tasks[pid].mEnabled = true;
}

void disable_task(uint32_t pid)
{
    tasks[pid].mEnabled = false;
}

void run_scheduler()
{
    while (true)
    {
        uint32_t next_wakeup = -1;
        for (auto &task : tasks)
        {
            if (!task.mEnabled)
                continue;
            next_wakeup = std::min(next_wakeup, task.maybe_resume());
        }
        if (millis() < next_wakeup)
            delay(next_wakeup - millis());
    }
}