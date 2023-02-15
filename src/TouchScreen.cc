#include "TouchScreen.hh"
#include "globals.hh"

constexpr auto touchscreen_sda = 33;
constexpr auto touchscreen_scl = 34;

constexpr auto ADDR = 0x15;

/**
 * All the commented-out stuff below is for the interrupt logic of the screen...
 * *Supposedly* a gesture should trigger the interrupt pin to go low, but that
 * never worked for me.
 * The chinese docs hint at the possibility of enabling those interrupts, but
 * the register writes over I2C don't appear to go through.
 * If I read the espruino code correctly, they're not bothering with the
 * interrupts either.
 */

DigitalOutPin resetPin{35};
// DigitalInPin interrupt{36, INPUT_PULLUP};

bool read_i2c_addr(uint8_t *data, uint32_t len, uint16_t addr, uint8_t reg_addr)
{
    Wire.beginTransmission(addr);
    Wire.write(reg_addr);
    if (Wire.endTransmission(true) != 0)
        return false;

    Wire.requestFrom(addr, len, true);
    for (int i = 0; i < len; i++)
        data[i] = Wire.read();
    return true;
}

// this doesn't appear to work unfortunately
// bool write_i2c_addr(uint8_t *data, uint32_t len, uint16_t addr, uint8_t reg_addr)
// {
//     Wire.beginTransmission(addr);
//     Wire.write(reg_addr);
//     for (int i = 0; i < len; i++)
//         Wire.write(data[i]);
//     return Wire.endTransmission(true) == 0;
// }

// struct IrqCtl // 0xFA
// {
//     uint8_t reserved : 4;
//     uint8_t once_wlp : 1;  // trigger interrupt on long-press
//     uint8_t en_change : 1; // trigger on a recognized gesture
//     uint8_t en_touch : 1;  // trigger on touch/release
//     uint8_t en_test : 1;   // trigger regularly (test mode)
// };
// static_assert(sizeof(IrqCtl) == 1, "IrqCtl is not 1 byte");

void TouchScreen::reset()
{
    pinMode(35, OUTPUT);
    pinMode(36, INPUT_PULLUP);

    resetPin = 1;
    delay(50);
    resetPin = 0;
    delay(5);
    resetPin = 1;
    delay(50);

    // IrqCtl trigger_on_gesture = {0, 0, 1, 0, 1};
    // uint8_t trigger_on_gesture = 0xff;
    // write_i2c_addr((uint8_t *)&trigger_on_gesture, 1, ADDR, 0xFA);

    // uint8_t sleep = 0x03;
    // write_i2c_addr(&sleep, 1, ADDR, 0xA5);

    // Wire.beginTransmission(ADDR);
    // Wire.write(0x03);
    // Wire.write(0xe5);
    // Wire.endTransmission(true);
}

void TouchScreen::signalEventsWaiting() { mEventsWaiting = true; }

// TouchScreen *touch_screen_instance = nullptr;
// void touchscreen_isr()
// {
//     vibrate = 1;
//     // if (touch_screen_instance)
//     //     touch_screen_instance->wakeup();
//     if (touch_screen_instance)
//         touch_screen_instance->signalEventsWaiting();
// }

TouchScreen::TouchScreen()
{
}

void TouchScreen::begin()
{
    Wire.begin();
    reset();

    // touch_screen_instance = this;

    // attachInterrupt(
    //     interrupt.pin, touchscreen_isr,
    //     FALLING);
}

TouchScreen::~TouchScreen()
{
    // touch_screen_instance = nullptr;
}

bool TouchScreen::pumpEvents()
{
    mInputFlags = 0;
    // if (mSleeping)
    //     return false;

    // if (!mEventsWaiting)
    //     return false;
    // mEventsWaiting = false;

    // I could get a whole struct of stuff here,
    // but I only really care about the gesture ID
    uint8_t gesture_id;
    if (!read_i2c_addr(&gesture_id, 1, ADDR, 0x01))
    {
        // mSleeping = true;
        return false;
    }
    // uint8_t no_gesture = 0x00;
    // write_i2c_addr(&no_gesture, 1, ADDR, 0x01);

    switch (gesture_id)
    {
    case 0x01:
        mInputFlags = InputFlag::SWIPE_DOWN;
        break;
    case 0x02:
        mInputFlags = InputFlag::SWIPE_UP;
        break;
    case 0x03:
        mInputFlags = InputFlag::SWIPE_LEFT;
        break;
    case 0x04:
        mInputFlags = InputFlag::SWIPE_RIGHT;
        break;
    case 0x05:
        mInputFlags = InputFlag::TAP;
        break;
    case 0x06:
        mInputFlags = InputFlag::DOUBLE_TAP;
        break;
    }

    // if (mInputFlags == 0)
    // {
    //     mEventsWaiting = false;
    //     reset();
    //     return false;
    // }

    return true;
}