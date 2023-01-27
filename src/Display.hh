#pragma once
// This file is based on the JDI_MIP_Display library by Giovanni Bertazzoni

#define DISPLAY_WIDTH 176  // Display width in pixel
#define DISPLAY_HEIGHT 176 // Display height in pixel
#define PIN_SCS 5
#define PIN_DISP 7
#define PIN_FRONTLIGHT 8

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "clean_arduino_macros.hh"

#define DIFF_LINE_UPDATE
#define BYTES_PER_LINE (DISPLAY_WIDTH / 2)

#define COLOR_BLACK 0x00
#define COLOR_BLUE 0x02
#define COLOR_GREEN 0x04
#define COLOR_CYAN 0x06
#define COLOR_RED 0x08
#define COLOR_MAGENTA 0x0a
#define COLOR_YELLOW 0x0c
#define COLOR_WHITE 0x0e

#define CMD_NO_UPDATE 0x00
#define CMD_BLINKING_BLACK 0x10
#define CMD_BLINKING_INVERSION 0x14
#define CMD_BLINKING_WHITE 0x18
#define CMD_ALL_CLEAR 0x20
#define CMD_VCOM 0x40
#define CMD_UPDATE 0x90

class Display : public Adafruit_GFX
{
public:
    Display();
    void begin();
    void refresh();
    void refreshLine(int line);
    void displayOn();
    void displayOff();
    void clearScreen();
    void frontlightOn();
    void frontlightOff();

    void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color, uint16_t width);

private:
    uint16_t _background;
    char _backBuffer[BYTES_PER_LINE * DISPLAY_HEIGHT];

    void sendLineCommand(char *line_cmd, int line);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
};

// MIT License
//
// Copyright(c) 2021 Giovanni Bertazzoni <nottheworstdev@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.