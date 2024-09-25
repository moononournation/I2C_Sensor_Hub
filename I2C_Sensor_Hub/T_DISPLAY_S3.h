#pragma once

// Display
#include <Arduino_GFX_Library.h>
#define GFX_EXTRA_PRE_INIT()          \
  {                                   \
    pinMode(15 /* PWD */, OUTPUT);    \
    digitalWrite(15 /* PWD */, HIGH); \
  }
#define GFX_BL 38
Arduino_DataBus *bus = new Arduino_ESP32PAR8Q(
    7 /* DC */, 6 /* CS */, 8 /* WR */, 9 /* RD */,
    39 /* D0 */, 40 /* D1 */, 41 /* D2 */, 42 /* D3 */, 45 /* D4 */, 46 /* D5 */, 47 /* D6 */, 48 /* D7 */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 5 /* RST */, 3 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */, 0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);
#define GFX_SPEED 80000000UL

// Button
#define BTN_A_PIN 0
#define BTN_B_PIN 14

// I2C
#define I2C_SDA 43
#define I2C_SCL 44
#define I2C_FREQ 400000UL

// Touchscreen
// #define TOUCH_MODULES_CST_SELF
// #define TOUCH_MODULE_ADDR CTS816S_SLAVE_ADDRESS
// #define TOUCH_SCL I2C_SCL
// #define TOUCH_SDA I2C_SDA
// #define TOUCH_RES -1
// #define TOUCH_INT -1

// SD card
// #define SD_SCK 3
// #define SD_MOSI 4 // CMD
// #define SD_MISO 2 // D0
// #define SD_D1 1
// #define SD_D2 6
// #define SD_CS 5   // D3

// I2S
// #define I2S_DEFAULT_GAIN_LEVEL 0.8
// #define I2S_OUTPUT_NUM I2S_NUM_0
// #define I2S_MCLK -1
// #define I2S_BCLK 18
// #define I2S_LRCK 16
// #define I2S_DOUT 17
// #define I2S_DIN -1

// #define AUDIO_MUTE_PIN 48   // LOW for mute
