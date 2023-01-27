#include "globals.hh"
#include "Scheduler.hh"
#include "Task.hh"

constexpr auto VIBRATE_PIN = 19; // == PIN 0.19
constexpr auto EXTCOMIN = 6;

Display display;
bool light_on = false;

DigitalOutPin vibrate{VIBRATE_PIN};
DigitalOutPin extcomin{EXTCOMIN};

Clock localclock;

Task battery_indicator_task()
{
    constexpr auto BATTERY_PIN = 3;
    analogReference(AR_DEFAULT); // 0.6V with 1/5 gain => 3V

    while (true)
    {
        const auto level = analogRead(BATTERY_PIN);
        auto voltage = level / 1024.f * 3.f * 4.f; // 4x voltage divider
        voltage = std::roundf(voltage * 100.f) / 100.0f;

        display.fillRect(0, 0, 176, TOPBAR_SIZE, COLOR_BLACK);
        display.setCursor(0, 0);
        display.print("Battery: ");
        display.print(voltage);
        display.print("V");

        for (auto i = 0; i < TOPBAR_SIZE; i++)
        {
            display.refreshLine(i);
            co_yield Task::resume_asap();
        }

        co_yield Task::resume_in(1000);
    }
}

Task frame_drawing_task()
{
    int frames = 0;
    auto color = COLOR_BLACK;

    while (true)
    {
        color = (color + 1) % 15;
        display.fillRect(0, TOPBAR_SIZE, 176, 176 - TOPBAR_SIZE, color);

        display.setCursor(30, 30);
        // display.setFont(&FreeMono12pt7b);
        display.print("Hello World!\n");
        display.setCursor(30, display.getCursorY());
        display.print("Frames: ");
        display.print(frames);
        display.refresh();

        frames++;

        co_yield Task::resume_in(1000);
    }
}

Task extcomin_flipping_task()
{
    static char last_state = LOW;
    while (true)
    {
        extcomin = (last_state ^= 1);
        co_yield Task::resume_in(light_on ? 5 : 250);
    }
}

Task deillumination_task()
{
    static auto last_lightup = millis();
    while (true)
    {
        if (millis() - last_lightup > 15000)
        {
            display.frontlightOff();
            light_on = false;
        }
        co_yield Task::resume_in(1000);
    }
}

void start_background_tasks()
{
    const auto start = [](Task &&t)
    {
        const auto pid = register_task(std::move(t));
        enable_task(pid);
    };

    start(battery_indicator_task());
    start(extcomin_flipping_task());
}
