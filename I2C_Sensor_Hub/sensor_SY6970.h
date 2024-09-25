#pragma once

#include <XPowersLib.h>

XPowersPPM sy6970;
bool sy6970_inited = false;
unsigned long next_sy6970_readings_ms = 0;
lv_obj_t *ui_sy6970_Chart;
lv_chart_series_t *ui_sy6970_Chart_Series;
lv_obj_t *ui_sy6970_box;
lv_obj_t *ui_sy6970_Label;
lv_color_t sy6970_color = lv_palette_main(LV_PALETTE_INDIGO);

void update_sy6970(uint8_t addr)
{
  if (!sy6970_inited)
  {
    if (!sy6970.init(Wire, I2C_SDA, I2C_SCL, addr))
    {
      Serial.println("Couldn't find SY6970 sensor");
    }
    else
    {
      // Set the minimum operating voltage. Below this voltage, the PPM will protect
      sy6970.setSysPowerDownVoltage(3300);

      // Set input current limit, default is 500mA
      sy6970.setInputCurrentLimit(3250);

      Serial.printf("getInputCurrentLimit: %d mA\n", sy6970.getInputCurrentLimit());

      // Disable current limit pin
      sy6970.disableCurrentLimitPin();

      // Set the charging target voltage, Range:3840 ~ 4608mV ,step:16 mV
      sy6970.setChargeTargetVoltage(4208);

      // Set the precharge current , Range: 64mA ~ 1024mA ,step:64mA
      sy6970.setPrechargeCurr(64);

      // The premise is that Limit Pin is disabled, or it will only follow the maximum charging current set by Limi tPin.
      // Set the charging current , Range:0~5056mA ,step:64mA
      sy6970.setChargerConstantCurr(1024);

      // Get the set charging current
      sy6970.getChargerConstantCurr();
      Serial.printf("getChargerConstantCurr: %d mA\n", sy6970.getChargerConstantCurr());

      // To obtain voltage data, the ADC must be enabled first
      sy6970.enableADCMeasure();

      // Turn on charging function
      // If there is no battery connected, do not turn on the charging function
      sy6970.enableCharge();

      // Turn off charging function
      // If USB is used as the only power input, it is best to turn off the charging function,
      // otherwise the VSYS power supply will have a sawtooth wave, affecting the discharge output capability.
      // sy6970.disableCharge();

      // The OTG function needs to enable OTG, and set the OTG control pin to HIGH
      // After OTG is enabled, if an external power supply is plugged in, OTG will be turned off

      // sy6970.enableOTG();
      // sy6970.disableOTG();
      // pinMode(OTG_ENABLE_PIN, OUTPUT);
      // digitalWrite(OTG_ENABLE_PIN, HIGH);

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_sy6970_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_sy6970_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_sy6970_box);
        lv_obj_set_style_bg_color(ui_sy6970_box, sy6970_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_sy6970_box, 1);

        ui_sy6970_Label = lv_label_create(ui_sy6970_box);
        lv_obj_set_size(ui_sy6970_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_sy6970_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_sy6970_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_sy6970_Label, "SY6970");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: SY6970 Power Management Sensor", addr);

        ui_sy6970_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_sy6970_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_sy6970_Chart);
        lv_obj_set_y(ui_sy6970_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_sy6970_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 5000);
        lv_chart_set_type(ui_sy6970_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_sy6970_Chart, CHART_POINT_COUNT);

        ui_sy6970_Chart_Series = lv_chart_add_series(ui_sy6970_Chart, sy6970_color, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        sy6970_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (sy6970_inited)
  {
    if (millis() > next_sy6970_readings_ms)
    {
      uint16_t batt = sy6970.getBattVoltage();
      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_sy6970_Chart, ui_sy6970_Chart_Series, batt);
        lv_chart_refresh(ui_sy6970_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_sy6970_Label, "Batt %04dmV", batt);
        xSemaphoreGive(lvglSemaphore);
      }

      next_sy6970_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
