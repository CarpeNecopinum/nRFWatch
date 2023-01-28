#include <vector>
#include <utility>

#include "Scheduler.hh"
#include "Task.hh"
#include <Arduino.h>
#include "clean_arduino_macros.hh"
#include "Inputs.hh"
#include "util.hh"
#include "globals.hh"

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
    inputs.setupInterrupts();
    while (true)
    {
        uint32_t next_wakeup = millis() + 0x80000000;
        InputFlags next_flags = 0;

        for (auto &task : tasks)
        {
            if (!task.mEnabled)
                continue;
            auto cond = task.maybe_resume();
            if (cond.at_millis != uint32_t(-1))
                next_wakeup = millis_min(next_wakeup, cond.at_millis);
            next_flags |= cond.inputs_mask;
        }

        // display.fillRect(0, 0, 176, 176, COLOR_BLUE);
        // display.setCursor(0, 20);
        // display.print(inputs.current());
        // display.refresh();
        // delay(1000);

        do
        {
            yield();
            inputs.pumpEvents();
            if (inputs.current() & next_flags)
                break;
        } while (millis_compare(millis(), next_wakeup) < 0);
    }
}