#pragma once

#include <SensorLTR553.hpp>

SensorLTR553 ltr553;
bool ltr553_inited = false;
unsigned long next_ltr553_readings_ms = 0;
lv_obj_t *ui_ltr553_Chart;
lv_chart_series_t *ui_ltr553_Chart_Series;
lv_obj_t *ui_ltr553_box;
lv_obj_t *ui_ltr553_Label;
lv_color_t ltr553_color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);

void update_ltr553(uint8_t addr)
{
  if (!ltr553_inited)
  {
    if (!ltr553.begin(Wire, addr, I2C_SDA, I2C_SCL))
    {
      Serial.println("Couldn't find LTR553 sensor");
    }
    else
    {
      // Set the ambient light high and low thresholds.
      // If the value exceeds or falls below the set value, an interrupt will be triggered.
      ltr553.setLightSensorThreshold(10, 200);

      // Set the high and low thresholds of the proximity sensor.
      // If the value exceeds or falls below the set value, an interrupt will be triggered.
      ltr553.setProximityThreshold(10, 30);

      // Controls the Light Sensor N number of times the measurement data is outside the range
      // defined by the upper and lower threshold limits before asserting the interrupt.
      ltr553.setLightSensorPersists(5);

      // Controls the Proximity  N number of times the measurement data is outside the range
      // defined by the upper and lower threshold limits before asserting the interrupt.
      ltr553.setProximityPersists(5);

      /*
       *  ALS_IRQ_ACTIVE_LOW, // INT pin is considered active when it is a logic 0 (default)
       *  ALS_IRQ_ACTIVE_HIGH // INT pin is considered active when it is a logic 1
       * * */
      ltr553.setIRQLevel(SensorLTR553::ALS_IRQ_ACTIVE_LOW);

      /*
       *  ALS_IRQ_ONLY_PS,    // Only PS measurement can trigger interrupt
       *  ALS_IRQ_ONLY_ALS,   // Only ALS measurement can trigger interrupt
       *  ALS_IRQ_BOTH,       // Both ALS and PS measurement can trigger interrupt
       * * * */
      ltr553.enableIRQ(SensorLTR553::ALS_IRQ_BOTH);

      /*
       *   ALS_GAIN_1X  ,   -> 1 lux to 64k lux (default)
       *   ALS_GAIN_2X  ,   -> 0.5 lux to 32k lux
       *   ALS_GAIN_4X  ,   -> 0.25 lux to 16k lux
       *   ALS_GAIN_8X  ,   -> 0.125 lux to 8k lux
       *   ALS_GAIN_48X ,   -> 0.02 lux to 1.3k lux
       *   ALS_GAIN_96X ,   -> 0.01 lux to 600 lux
       * */
      ltr553.setLightSensorGain(SensorLTR553::ALS_GAIN_8X);

      /*
       *   PS_LED_PLUSE_30KHZ,
       *   PS_LED_PLUSE_40KHZ,
       *   PS_LED_PLUSE_50KHZ,
       *   PS_LED_PLUSE_60KHZ,
       *   PS_LED_PLUSE_70KHZ,
       *   PS_LED_PLUSE_80KHZ,
       *   PS_LED_PLUSE_90KHZ,
       *   PS_LED_PLUSE_100KHZ,
       * * * * * * * * * * */
      ltr553.setPsLedPulsePeriod(SensorLTR553::PS_LED_PLUSE_100KHZ);

      /*
       *   PS_LED_DUTY_25,
       *   PS_LED_DUTY_50,
       *   PS_LED_DUTY_75,
       *   PS_LED_DUTY_100,
       * * * */
      ltr553.setPsLedDutyCycle(SensorLTR553::PS_LED_DUTY_100);

      /*
       *   PS_LED_CUR_5MA,
       *   PS_LED_CUR_10MA,
       *   PS_LED_CUR_20MA,
       *   PS_LED_CUR_50MA,
       *   PS_LED_CUR_100MA,
       * * * * * * * */
      ltr553.setPsLedCurrent(SensorLTR553::PS_LED_CUR_50MA);

      /*
       *   PS_MEAS_RATE_50MS,
       *   PS_MEAS_RATE_70MS,
       *   PS_MEAS_RATE_100MS,
       *   PS_MEAS_RATE_200MS,
       *   PS_MEAS_RATE_500MS,
       *   PS_MEAS_RATE_1000MS,
       *   PS_MEAS_RATE_2000MS,
       *   PS_MEAS_RATE_10MS
       * * * * * * * */
      ltr553.setProximityRate(SensorLTR553::PS_MEAS_RATE_200MS);

      // Number of pulses
      ltr553.setPsLedPulses(1);

      // Enable proximity sensor saturation indication
      ltr553.enablePsIndicator();

      // Enable ambient light sensor
      ltr553.enableLightSensor();

      // Enable proximity sensor
      ltr553.enableProximity();

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_ltr553_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_ltr553_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_ltr553_box);
        lv_obj_set_style_bg_color(ui_ltr553_box, ltr553_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_ltr553_box, 1);

        ui_ltr553_Label = lv_label_create(ui_ltr553_box);
        lv_obj_set_size(ui_ltr553_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_ltr553_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_ltr553_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_ltr553_Label, "LTR553");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: LTR553 Optical Sensor", addr);

        ui_ltr553_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_ltr553_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_ltr553_Chart);
        lv_obj_set_y(ui_ltr553_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_ltr553_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 2048);
        lv_chart_set_type(ui_ltr553_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_ltr553_Chart, CHART_POINT_COUNT);

        ui_ltr553_Chart_Series = lv_chart_add_series(ui_ltr553_Chart, ltr553_color, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        ltr553_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (ltr553_inited)
  {
    if (millis() > next_ltr553_readings_ms)
    {
      bool saturated = false;
      int raw = ltr553.getProximity(&saturated);

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_ltr553_Chart, ui_ltr553_Chart_Series, raw);
        lv_chart_refresh(ui_ltr553_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_ltr553_Label, "Prx %d", raw);
        xSemaphoreGive(lvglSemaphore);
      }

      next_ltr553_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
