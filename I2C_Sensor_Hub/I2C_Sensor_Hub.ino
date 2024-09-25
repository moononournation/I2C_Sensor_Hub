/*******************************************************************************
   I2C Sensors Hub

   Dependent libraries:
   LVGL V8: https://github.com/lvgl/lvgl/tree/release/v8.4

   LVGL Configuration file:
   Copy your_arduino_path/libraries/lvgl/lv_conf_template.h
   to your_arduino_path/libraries/lv_conf.h

   In lv_conf.h around line 15, enable config file:
   #if 1 // Set it to "1" to enable content

   Then find and set:
   #define LV_COLOR_DEPTH        16

   Customize font size:
   #define LV_FONT_MONTSERRAT_20 1
   #define LV_FONT_MONTSERRAT_24 1

   For LVGL V8:
   #define LV_TICK_CUSTOM        1
   #define LV_SPRINTF_USE_FLOAT  1

   For LVGL V9:
   #define LV_USE_FLOAT          1
 ******************************************************************************/
#define LV_HEADER_SIZE 40
#define LV_FONT_HEADER &lv_font_montserrat_20
#define LV_FONT_FIGURE &lv_font_montserrat_24
#define NUMBER_OF_ROW 3
#define CHART_POINT_COUNT 30
#define READINGS_MS_INTERVAL 1000

#include <Wire.h>

// Uncomment one and only one dev board pins definition
// #include "T_DISPLAY_S3.h"
// #include "T_DISPLAY_S3_AMOLED.h"
#include "T_DISPLAY_S3_PRO.h"

#include "lvgl_func.h"
#include "sensors_func.h"

void core0_task(void *pvParam)
{
  while (1) // infinite loop
  {
    lvgl_loop();

    vTaskDelay(pdMS_TO_TICKS(5));
  }
  vTaskDelete(NULL);
}

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while (!Serial);
  Serial.println("I2C Seonsor Hub");

  Wire.begin(I2C_SDA, I2C_SCL, I2C_FREQ);

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!gfx->begin(GFX_SPEED))
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  lvgl_init();

  // Pin LVGL loop to Core 0
  xTaskCreatePinnedToCore(
      (TaskFunction_t)core0_task,
      (const char *const)"Core 0 task",
      (const uint32_t)8192,
      (void *const)NULL,
      (UBaseType_t)configMAX_PRIORITIES - 1,
      (TaskHandle_t *const)NULL,
      (const BaseType_t)0);

  Serial.println("Setup done");
}

void loop()
{
  sensors_loop();
}
