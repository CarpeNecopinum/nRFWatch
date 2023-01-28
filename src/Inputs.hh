#pragma once

#include <stdint.h>

struct InputFlag
{
    enum
    {
        BUTTON_PRESS = 1 << 0,
        BUTTON_RELEASE = 1 << 1,
        SWIPE_LEFT = 1 << 2,
        SWIPE_RIGHT = 1 << 3,
        SWIPE_UP = 1 << 4,
        SWIPE_DOWN = 1 << 5,
        TAP = 1 << 6
    };
};
using InputFlags = uint8_t;

class Inputs
{
public:
    Inputs();
    uint8_t current() const { return mFlags; }

    // void enable();
    // void disable();

    void pumpEvents();
    void setupInterrupts();

private:
    InputFlags mFlags = 0;
    // uint32_t mPid = -1;
};