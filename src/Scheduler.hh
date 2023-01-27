#pragma once
#include <stdint.h>

struct Task;

uint32_t register_task(Task &&task);

void run_scheduler();
void disable_task(uint32_t pid);
void enable_task(uint32_t pid);