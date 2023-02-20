#include "globals.hh"
#include "Scheduler.hh"
#include "Task.hh"
#include "Inputs.hh"

#include <nrf52.h>

constexpr auto VIBRATE_PIN = 19; // == PIN 0.19
constexpr auto EXTCOMIN = 6;

Display display;
bool light_on = false;

DigitalOutPin vibrate{VIBRATE_PIN};
//

Clock localclock;

Inputs inputs;

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
        display.print("   I: ");
        display.print(inputs.current());

        for (auto i = 0; i < TOPBAR_SIZE; i++)
        {
            display.refreshLine(i);
            // co_yield Task::resume_asap();
        }

        // co_yield Task::resume_in(1000);
        co_yield Task::resume_asap();
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

void setup_extcomin_pwm()
{
    pinMode(EXTCOMIN, OUTPUT);
    analogWrite(EXTCOMIN, 0);

    auto pwm = NRF_PWM2;
    pwm->PSEL.OUT[0] = EXTCOMIN;
    pwm->PSEL.OUT[1] = EXTCOMIN;
    pwm->PSEL.OUT[2] = EXTCOMIN;
    pwm->PSEL.OUT[3] = EXTCOMIN;
    pwm->ENABLE = 1;
    pwm->PRESCALER = PWM_PRESCALER_PRESCALER_DIV_128; //(125kHz)
    pwm->MODE = PWM_MODE_UPDOWN_Up;
    // 120Hz frequency
    pwm->COUNTERTOP = 125000 / 60;
    pwm->LOOP = 0;
    pwm->DECODER = ((uint32_t)PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) | ((uint32_t)PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
    static uint16_t seq = pwm->COUNTERTOP >> 1;
    pwm->SEQ[0].PTR = (uint32_t)&seq;
    pwm->SEQ[0].CNT = 1;
    pwm->SEQ[0].REFRESH = 1;
    pwm->SEQ[0].ENDDELAY = 0;
    pwm->TASKS_SEQSTART[0] = 0x1UL;
}

// This was cool as a starting off point and an excuse to build fine-grained scheduling
// but it's inferior to just abusing PWM for the EXTCOMIN pin.
// Task extcomin_flipping_task()
// {
//     static DigitalOutPin extcomin{EXTCOMIN};
//     static char last_state = LOW;

//     while (true)
//     {
//         extcomin = (last_state ^= 1);
//         co_yield Task::resume_in(light_on ? 5 : 250);
//     }
// }

Task deillumination_task()
{
    constexpr auto darken_after = 60000;

    static auto last_lightup = millis();
    while (true)
    {
        co_yield ContinuationCondition{
            millis() + darken_after,
            0 | InputFlag::BUTTON_PRESS};

        if (inputs.current() & InputFlag::BUTTON_PRESS)
        {
            last_lightup = millis();
            if (!light_on)
            {
                display.frontlightOn();
                light_on = true;
            }
        }

        if (millis() - last_lightup >= darken_after)
        {
            display.frontlightOff();
            light_on = false;
        }
    }
}

void start_background_tasks()
{
    const auto start = [](Task &&t)
    {
        const auto pid = register_task(std::move(t));
        enable_task(pid);
    };

    setup_extcomin_pwm();
    start(battery_indicator_task());
    // start(extcomin_flipping_task());
    start(deillumination_task());
}
