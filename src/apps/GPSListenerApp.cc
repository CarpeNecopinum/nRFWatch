#include "GPSListenerApp.hh"
#include "../Scheduler.hh"
#include "../globals.hh"
#include "../Task.hh"
#include <TinyGPSPlus.h>

#include <string>

DigitalOutPin gps_power(29);
TinyGPSPlus gps;

constexpr auto max_chars = (176 * 176) / (8 * 8);

GPSListenerApp::GPSListenerApp()
{

    mPid = register_task(([&]() -> Task
                          {
        Serial.begin(9600);
        std::string gps_data;

        constexpr auto bg = COLOR_BLACK;
        while (true)
        {
            if (Serial.available()) {
                while (Serial.available()) {
                    char c = Serial.read();
                    gps.encode(c);
                    if (gps_data.size() > max_chars)
                        gps_data.erase(0, 1);
                    gps_data += c;
                }

                if (gps.time.isValid()) {
                    display.fillRect(0, TOPBAR_SIZE, 176, 176 - TOPBAR_SIZE, bg);

                    display.setCursor(0, TOPBAR_SIZE);

                    display.print("Time:  ");
                    display.print(gps.time.hour());
                    display.print(":");
                    display.print(gps.time.minute());
                    display.print(":");
                    display.print(gps.time.second());
                    display.print("\n\r");

                    display.print("Date:  ");
                    display.print(gps.date.day());
                    display.print(".");
                    display.print(gps.date.month());
                    display.print(".");
                    display.print(gps.date.year());
                    display.print("\n\r");

                    display.print("Age: ");
                    display.print(gps.time.age());
                    display.print("\n\r");

                    display.print("Lat: ");
                    display.print(gps.location.lat(), 6);
                    display.print("\n\r");

                    display.print("Lng: ");
                    display.print(gps.location.lng(), 6);
                    display.print("\n\r");

                    display.print("\n");
                    display.print(gps_data.c_str());
   
                    // for (int i = TOPBAR_SIZE; i < 176; i++) {
                    //     display.refreshLine(i);
                    //     if (i % 10 == 9)
                    //         co_yield Task::resume_asap();
                    // }       
                    display.refresh();
                } 

                // display.fillRect(0, TOPBAR_SIZE, 176, 176 - TOPBAR_SIZE, bg);
                // display.setCursor(0, TOPBAR_SIZE);
                // display.print(gps_data.c_str());
                // for (int i = TOPBAR_SIZE; i < 176; i++) {
                //     display.refreshLine(i);
                //     if (i % 10 == 9)
                //         co_yield Task::resume_asap();
                // }
            }
            co_yield Task::resume_in(250);
        } })());
}

void GPSListenerApp::enable()
{
    gps_power = 1;
    enable_task(mPid);
}

void GPSListenerApp::disable()
{
    gps_power = 0;
    disable_task(mPid);
}
