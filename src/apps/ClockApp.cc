#include "ClockApp.hh"
#include "globals.hh"
#include "../Scheduler.hh"
#include "../Task.hh"

ClockApp::ClockApp()
{
    mPid = register_task(([this]() -> Task { //
        static auto frames = 0;

        // constexpr auto BG = COLOR_BLACK;
        // constexpr auto center_x = 176 / 2;
        // constexpr auto center_y = 176 / 2;

        constexpr auto bg = COLOR_BLACK;
        while (true)
        {
            display.fillRect(0, TOPBAR_SIZE, 176, 176 - TOPBAR_SIZE, bg);

            const auto time = localclock.getTime();

            const auto center = 176 / 2;
            for (auto i = 0; i < 12; i++) // draw hour-ticks
            {
                const auto rads = i * 30 * M_PI / 180;
                display.writeLine(center + sin(rads) * 60, center - cos(rads) * 60,
                                  center + sin(rads) * 70, center - cos(rads) * 70,
                                  COLOR_CYAN, 3);
            }
            for (auto i = 0; i < 60; i++) // draw minute-ticks
            {
                const auto rads = i * 6 * M_PI / 180;
                display.drawLine(center + sin(rads) * 60, center - cos(rads) * 60,
                                 center + sin(rads) * 65, center - cos(rads) * 65,
                                 COLOR_CYAN);
            }

            { // draw hour hand
                const auto angle = (time.tm_hour % 12) * 30 + time.tm_min / 2;
                const auto x = center + sin(angle * M_PI / 180) * 40;
                const auto y = center - cos(angle * M_PI / 180) * 40;
                display.writeLine(center, center, x, y, COLOR_WHITE, 5);
            }
            { // draw minute hand
                const auto angle = time.tm_min * 6 + time.tm_sec / 10;
                const auto x = center + sin(angle * M_PI / 180) * 60;
                const auto y = center - cos(angle * M_PI / 180) * 60;
                display.writeLine(center, center, x, y, COLOR_WHITE, 3);
            }
            { // draw second hand
                const auto angle = time.tm_sec * 6;
                const auto x = center + sin(angle * M_PI / 180) * 60;
                const auto y = center - cos(angle * M_PI / 180) * 60;
                display.drawLine(center, center, x, y, COLOR_RED);
            }

            // display.print(time.tm_hour);
            // display.print(":");
            // display.print(time.tm_min);
            // display.print(":");
            // display.print(time.tm_sec);

            for (auto i = TOPBAR_SIZE; i < DISPLAY_HEIGHT; i++)
            {
                display.refreshLine(i);
                if (i % 10 == 9)
                    co_yield Task::resume_asap();
            }

            display.refresh();

            frames++;

            co_yield Task::resume_at(localclock.nextSecondMillis());
        }
    })());
}

void ClockApp::enable()
{
    enable_task(mPid);
}

void ClockApp::disable()
{
    disable_task(mPid);
}