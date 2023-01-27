#pragma once

#include <time.h>

struct TimeInput
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

class Clock
{
public:
    Clock();

    void update();

    time_t getTimestamp();
    time_t getTimestamp() const;

    void setTimestamp(time_t timestamp);
    void setTime(TimeInput time);

    tm getTime();
    tm getTime() const;

    uint32_t nextSecondMillis()
    {
        return mLastUpdateMillis + 1000;
    }

private:
    time_t mLastUpdateTime;
    uint32_t mLastUpdateMillis;
};