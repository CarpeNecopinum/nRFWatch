#pragma once
#include <array>
#include "Scheduler.hh"
#include "apps/IApp.hh"

class AppSwitcher
{
public:
    /// use this if your app needs the up/down swipe gestures for itself
    /// don't forget to re-enable() the switcher when you're done
    static void disable()
    {
        disable_task(mPid);
    }
    static void enable()
    {
        enable_task(mPid);
    }

    static void register_app(IApp *app)
    {
        mApps[mNumApps++] = app;
        // mApps.push_back(std::move(app));
    }

    static void init();

private:
    static uint32_t mPid;
    static std::array<IApp *, 8> mApps;
    static uint32_t mNumApps;

    static IApp *mCurrentApp;
};