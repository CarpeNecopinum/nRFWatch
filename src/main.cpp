
#include "Task.hh"
#include "Scheduler.hh"
#include "globals.hh"

#include <vector>
#include <array>
#include <list>

#include "apps/ClockApp.hh"
#include "apps/GPSListenerApp.hh"

void setup()
{
    display.begin();
    delay(50);
    display.displayOn();
    display.frontlightOn(); // Optional depending on the display model
    light_on = true;
    display.clearScreen(); // Clear the screen
    display.refresh();     // Actually updates the display

    start_background_tasks();

    localclock.setTime({
        .year = 2023,
        .month = 1,
        .day = 27,
        .hour = 17,
        .minute = 24,
        .second = 0,
    });

    // ClockApp clock;
    // clock.enable();

    GPSListenerApp gps;
    gps.enable();

    run_scheduler();
}

void loop()
{
    // should never be reached
    display.fillRect(0, 0, 176, 176, COLOR_RED);
    display.setCursor(30, 30);
    display.print("You should never see this.");
    display.refresh();
}