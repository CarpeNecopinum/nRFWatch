// This file is based on the JDI_MIPPIN_DISPlay library by Giovanni Bertazzoni
// some of the changes I made:
// - removed unnecassarily copied pin numbers, as they are constant anyways
// - removed the _background member, as it's only used in ::begin() and ::setBackgroundColor()
// - drop the _dispBuffer, will follow up with a different scheme to reduce over-sending

#include <utility>
#include "Display.hh"

Display::Display() : Adafruit_GFX(DISPLAY_WIDTH, DISPLAY_HEIGHT)
{
}

void Display::begin()
{
    digitalWrite(PIN_SCS, LOW);
    pinMode(PIN_SCS, OUTPUT);
    pinMode(PIN_DISP, OUTPUT);
    pinMode(PIN_FRONTLIGHT, OUTPUT);

    auto bg = COLOR_BLACK;
    memset(&_backBuffer[0], (char)((bg & 0x0F) | ((bg & 0x0F) << 4)), sizeof(_backBuffer));
    SPI.begin();
}

void Display::refresh()
{
    for (int i = 0; i < height(); i++)
        refreshLine(i);
}

void Display::refreshLine(int line)
{
    int line_start_offset = BYTES_PER_LINE * line;
    char *line_cmd = &_backBuffer[line_start_offset];
    sendLineCommand(line_cmd, line);
}

void Display::clearScreen()
{
    digitalWrite(PIN_SCS, HIGH);
    SPI.transfer(CMD_ALL_CLEAR);
    SPI.transfer(0x00);
    digitalWrite(PIN_SCS, LOW);
}

void Display::sendLineCommand(char *line_cmd, int line)
{
    if ((line < 0) || (line >= height()))
    {
        return;
    }

    digitalWrite(PIN_SCS, HIGH);
    SPI.transfer(CMD_UPDATE);
    SPI.transfer(line + 1);

    for (int i = 0; i < BYTES_PER_LINE; i++)
    {
        SPI.transfer(line_cmd[i]);
    }

    SPI.transfer(0x00);
    SPI.transfer(0x00);
    digitalWrite(PIN_SCS, LOW);
}

void Display::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || x >= width() || y < 0 || y >= height())
    {
        return;
    }

    int pixelIdx = ((width() / 2) * y) + (x / 2);

    if (x % 2 == 0)
    {
        _backBuffer[pixelIdx] &= 0x0F;
        _backBuffer[pixelIdx] |= (color & 0x0F) << 4;
    }
    else
    {
        _backBuffer[pixelIdx] &= 0xF0;
        _backBuffer[pixelIdx] |= color & 0x0F;
    }
}

void Display::displayOn()
{
    digitalWrite(PIN_DISP, HIGH);
}

void Display::displayOff()
{
    digitalWrite(PIN_DISP, LOW);
}

void Display::frontlightOn()
{
    digitalWrite(PIN_FRONTLIGHT, HIGH);
}

void Display::frontlightOff()
{
    digitalWrite(PIN_FRONTLIGHT, LOW);
}

void Display::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                        uint16_t color, uint16_t width)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            for (int dy = -width / 2; dy <= width / 2; dy++)
                writePixel(y0 + dy, x0, color);
        }
        else
        {
            for (int dy = -width / 2; dy <= width / 2; dy++)
                writePixel(x0, y0 + dy, color);
        }
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

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