#pragma once
#include "IApp.hh"
#include <stdint.h>

class GPSListenerApp : public IApp
{
public:
    GPSListenerApp();
    ~GPSListenerApp() override = default;
    void enable() override;
    void disable() override;

private:
    uint32_t mPid;
};