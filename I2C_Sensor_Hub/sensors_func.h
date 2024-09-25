#pragma once

#include "sensor_ADS1x15.h"
#include "sensor_LTR390.h"
#include "sensor_LTR553.h"
#include "sensor_PCF85063.h"
#include "sensor_PCT2075.h"
#include "sensor_SGP40.h"
#include "sensor_SHT4x.h"
#include "sensor_SY6970.h"
#include "sensor_TSL2591.h"
#include "sensor_VCNL4040.h"
#include "sensor_VEML7700.h"

typedef void(SENSOR_FUNC)(uint8_t addr);

// dummy function for skip the detected addr
void addr_skip(uint8_t) {}

SENSOR_FUNC *func_list[] = {
    /* 0x00-0F */
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    /* 0x10-1F */
    update_veml7700,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    /* 0x20-2F */
    nullptr,
    nullptr,
    nullptr,
    update_ltr553,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    update_tsl2591,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    /* 0x30-3F */
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    update_pct2075,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    /* 0x40-4F */
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    update_sht4x,
    nullptr,
    nullptr,
    nullptr,
    update_ads1x15,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    /* 0x50-5F */
    nullptr,
    update_pcf85063,
    nullptr,
    update_ltr390,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    update_sgp40,
    addr_skip, // skip CSTxxx touchscreen
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    /* 0x60-6F */
    update_vcnl4040,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    update_sy6970,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    /* 0x70-7F */
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

void sensors_loop()
{
  // Serial.println("Scanning for I2C devices ...");
  SENSOR_FUNC *func;
  uint8_t error;
  for (uint8_t address = 0x01; address < 0x7f; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      // Serial.printf("I2C device found at address 0x%02X\n", address);
      func = func_list[address];
      if (func)
      {
        (*func)(address);
      }
      else
      {
        Serial.printf("Not define func for address 0x%02X\n", address);
      }
    }
    else if (error != 2)
    {
      Serial.printf("Error %d at address 0x%02X\n", error, address);
    }
  }
}
