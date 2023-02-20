#pragma once

#include <Wire.h>
#include "Inputs.hh"

class TouchScreen
{
public:
    TouchScreen();
    ~TouchScreen();

    void begin();

    bool pumpEvents();
    InputFlags lastEvent() const { return mInputFlags; }

    void signalEventsWaiting();

    bool sleeping() const { return mSleeping; }

    // only for the ISR really
    void wakeup() { mSleeping = false; }

private:
    void reset();

    bool mEventsWaiting = false;
    bool mSleeping = false;
    InputFlags mInputFlags;
    InputFlags mOldFlags = 0;
};