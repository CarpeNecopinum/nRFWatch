#pragma once
#include "IApp.hh"
#include <stdint.h>

class ClockApp : public IApp
{
public:
    ClockApp();
    ~ClockApp() override = default;
    void enable() override;
    void disable() override;

private:
    uint32_t mPid;
};