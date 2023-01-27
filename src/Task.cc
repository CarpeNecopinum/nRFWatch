#include "Task.hh"
#include <Arduino.h>
#include "clean_arduino_macros.hh"

bool ContinuationCondition::should_resume() const
{
    switch (type)
    {
    case Type::Immediately:
        return true;
    case Type::Millis:
        return mData.millis <= millis();
    default:
        return true;
    }
}

ContinuationCondition Task::resume_at(uint32_t millis)
{
    return ContinuationCondition::Millis(millis);
}
ContinuationCondition Task::resume_asap()
{
    return ContinuationCondition::Immediately();
}
ContinuationCondition Task::resume_in(uint32_t _millis)
{
    return ContinuationCondition::Millis(millis() + _millis);
}