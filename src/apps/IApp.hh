#pragma once

class IApp
{
public:
    virtual ~IApp() = default;
    virtual void enable() = 0;
    virtual void disable() = 0;
};