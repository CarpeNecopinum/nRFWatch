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

struct TouchInfo
{
    uint8_t reserved1;
    uint8_t gesture_id;
    uint8_t event_type : 2;
    uint8_t reserved2 : 2;
    uint8_t x_msb : 4;
    uint8_t x_lsb;
    uint8_t reserved3 : 4;
    uint8_t y_msb : 4;
    uint8_t y_lsb;

    uint8_t dummy[3];
};
static_assert(sizeof(TouchInfo) == 9, "touch_state is the wrong number of bytes");

bool TouchScreen::pumpEvents()
{
    mInputFlags = 0;

    if (!light_on)
        return false;

    // if (mSleeping)
    //     return false;

    // if (!mEventsWaiting)
    //     return false;
    // mEventsWaiting = false;

    // I could get a whole struct of stuff here,
    // but I only really care about the gesture ID
    // TouchInfo touch_state;

    // if (!read_i2c_addr((uint8_t *)&touch_state, sizeof(touch_state), ADDR, 0))
    // {
    //     // mSleeping = true;
    //     return false;
    // }

    // uint8_t no_gesture = 0x00;
    // write_i2c_addr(&no_gesture, 1, ADDR, 0x01);

    char buf[9] = {0};
    if (!read_i2c_addr((uint8_t *)buf, 9, ADDR, 0))
        return false;

    uint8_t gesture = buf[1];
    uint8_t event = buf[3] >> 6;
    uint16_t x = ((buf[3] & 0x0f) << 8) | buf[4];
    uint16_t y = ((buf[5] & 0x0f) << 8) | buf[6];

    uint8_t flag = 0;
    if (event == 0x01 /* UP */)
    {
        switch (gesture)
        {
        case 0x01:
            flag = InputFlag::SWIPE_DOWN;
            break;
        case 0x02:
            flag = InputFlag::SWIPE_UP;
            break;
        case 0x03:
            flag = InputFlag::SWIPE_LEFT;
            break;
        case 0x04:
            flag = InputFlag::SWIPE_RIGHT;
            break;
        case 0x05:
            flag = InputFlag::TAP;
            break;
        case 0x06:
            flag = InputFlag::DOUBLE_TAP;
            break;
        }
    }

    // Don't send repeated events
    if (flag != mOldFlags)
    {
        mInputFlags = flag;
        mOldFlags = flag;
    }

    // display.fillRect(0, TOPBAR_SIZE, 176, 176 - TOPBAR_SIZE, COLOR_RED);
    // display.setCursor(0, 12);
    // display.setTextColor(COLOR_WHITE);
    // display.setTextSize(2);
    // display.print(gesture);
    // display.print("\n");
    // display.print(event);
    // display.print("\n");
    // display.print(x);
    // display.print("\n");
    // display.print(y);
    // display.print("\n");
    // display.print(mInputFlags);
    // display.print("\n");
    // display.print(mOldFlags);
    // display.print("\n");
    // display.setTextSize(1);
    // display.refresh();
    // delay(300);

    // if (mInputFlags == 0)
    // {
    //     mEventsWaiting = false;
    //     reset();
    //     return false;
    // }

    return true;
}