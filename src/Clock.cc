#include "Clock.hh"
#include <Arduino.h>

Clock::Clock()
{
    mLastUpdateTime = 0;
    mLastUpdateMillis = 0;
}

void Clock::update()
{
    // explicit uint32_t so overflows work to our favor
    uint32_t now = millis();
    uint32_t delta = now - mLastUpdateMillis;

    auto delta_seconds = delta / 1000;
    mLastUpdateTime += delta_seconds;

    mLastUpdateMillis += delta_seconds * 1000;
}

time_t Clock::getTimestamp()
{
    update();
    return mLastUpdateTime;
}

time_t Clock::getTimestamp() const
{
    return mLastUpdateTime + (millis() - mLastUpdateMillis) / 1000;
}

void Clock::setTimestamp(time_t timestamp)
{
    mLastUpdateTime = timestamp;
    mLastUpdateMillis = 0;
}

tm Clock::getTime()
{
    update();
    return *localtime(&mLastUpdateTime);
}

tm Clock::getTime() const
{
    const time_t now = mLastUpdateTime + (millis() - mLastUpdateMillis) / 1000;
    return *localtime(&now);
}

void Clock::setTime(TimeInput time)
{
    tm timeinfo;
    timeinfo.tm_year = time.year;
    timeinfo.tm_mon = time.month;
    timeinfo.tm_mday = time.day;
    timeinfo.tm_hour = time.hour;
    timeinfo.tm_min = time.minute;
    timeinfo.tm_sec = time.second;
    timeinfo.tm_isdst = -1;

    mLastUpdateTime = mktime(&timeinfo);
    mLastUpdateMillis = millis();
}