#pragma once

#include <memory>
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
        TAP = 1 << 6,
        DOUBLE_TAP = 1 << 7,
    };
};
using InputFlags = uint8_t;

class TouchScreen;
class Inputs
{
public:
    Inputs();
    ~Inputs();
    uint8_t current() const { return mFlags; }

    void begin();

    // void enable();
    // void disable();

    void pumpEvents();

private:
    InputFlags mFlags = 0;
    std::unique_ptr<TouchScreen> mScreen;
    void setupInterrupts();
    // uint32_t mPid = -1;
};