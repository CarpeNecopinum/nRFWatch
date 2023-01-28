#include "Task.hh"
#include <Arduino.h>
#include "clean_arduino_macros.hh"
#include "util.hh"
#include "globals.hh"

bool ringedGreaterEqual(uint32_t a, uint32_t b)
{
    // "estimate" of a > b, based on the knowledge that
    // an overflow may have occurred between them

    return (a >= b) == (a - b < 0x80000000);
}

bool ContinuationCondition::should_resume() const
{
    if (at_millis != -1 && (millis_compare(millis(), at_millis) > 0))
        return true;
    return (inputs_mask & inputs.current());
}

ContinuationCondition Task::resume_at(uint32_t millis)
{
    // -1 is reserved for "never"
    if (millis == -1)
        millis = -2;
    return ContinuationCondition{millis, 0};
}
ContinuationCondition Task::resume_asap()
{
    return ContinuationCondition{millis(), 0};
}
ContinuationCondition Task::resume_in(uint32_t _millis)
{
    return resume_at(millis() + _millis);
}

Task::promise_type::promise_type()
{
    mCondition = Task::resume_asap();
}