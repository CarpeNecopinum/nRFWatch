#include "Inputs.hh"
#include <Arduino.h>

#include "globals.hh"

volatile InputFlags mNextFlags = 0;

void button_ISR()
{
    mNextFlags |= uint8_t(InputFlag::BUTTON_PRESS);
}

void Inputs::setupInterrupts()
{
    static bool initialized = false;
    if (initialized)
        return;

    pinMode(17, INPUT_PULLUP);
    attachInterrupt(17, button_ISR, FALLING);

    initialized = true;
}

Inputs::Inputs()
{
    setupInterrupts();
}

void Inputs::pumpEvents()
{
    mFlags = mNextFlags;
    mNextFlags = 0;
}

// void Inputs::enable()
// {
//     if (mPid == -1)
//         mPid = register_task([this]() -> Task { //
//             while (true)
//             {
//                 pumpEvents();
//                 co_yield Task::resume_asap();
//             }
//         });

//     enable_task(mPid);
// }

// void Inputs::disable()
// {
//     disable_task(mPid);
// }