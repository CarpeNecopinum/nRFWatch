#pragma once

#include "Display.hh"
#include "Clock.hh"
#include "Inputs.hh"

struct DigitalOutPin
{
    DigitalOutPin(uint8_t pin) : mPin(pin)
    {
        pinMode(mPin, OUTPUT);
    }
    DigitalOutPin &operator=(bool value)
    {
        digitalWrite(mPin, value);
        return *this;
    }

    uint32_t mPin;
};

extern DigitalOutPin vibrate;
extern DigitalOutPin extcomin;

constexpr auto TOPBAR_SIZE = 8;

extern Display display;
extern bool light_on;

void start_background_tasks();

extern Clock localclock;

extern Inputs inputs;