#pragma once

#include <Adafruit_VCNL4040.h>

Adafruit_VCNL4040 vcnl4040;
bool vcnl4040_inited = false;
unsigned long next_vcnl4040_readings_ms = 0;
lv_obj_t *ui_vcnl4040_Chart;
lv_chart_series_t *ui_vcnl4040_Chart_Series;
lv_obj_t *ui_vcnl4040_box;
lv_obj_t *ui_vcnl4040_Label;
lv_color_t vcnl4040_color = lv_palette_main(LV_PALETTE_CYAN);

void update_vcnl4040(uint8_t addr)
{
  if (!vcnl4040_inited)
  {
    vcnl4040 = Adafruit_VCNL4040();
    if (!vcnl4040.begin())
    {
      Serial.println("Couldn't find VCNL4040 sensor");
    }
    else
    {
      vcnl4040.setProximityLEDCurrent(VCNL4040_LED_CURRENT_50MA);
      vcnl4040.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_40);
      vcnl4040.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);
      vcnl4040.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
      vcnl4040.setProximityHighResolution(false);

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_vcnl4040_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_vcnl4040_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_vcnl4040_box);
        lv_obj_set_style_bg_color(ui_vcnl4040_box, vcnl4040_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_vcnl4040_box, 1);

        ui_vcnl4040_Label = lv_label_create(ui_vcnl4040_box);
        lv_obj_set_size(ui_vcnl4040_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_vcnl4040_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_vcnl4040_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_vcnl4040_Label, "VCNL4040");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: VCNL4040 Proximity Sensor", addr);

        ui_vcnl4040_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_vcnl4040_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_vcnl4040_Chart);
        lv_obj_set_y(ui_vcnl4040_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_vcnl4040_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 4096);
        lv_chart_set_type(ui_vcnl4040_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_vcnl4040_Chart, CHART_POINT_COUNT);

        ui_vcnl4040_Chart_Series = lv_chart_add_series(ui_vcnl4040_Chart, vcnl4040_color, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        vcnl4040_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (vcnl4040_inited)
  {
    if (millis() > next_vcnl4040_readings_ms)
    {
      uint16_t raw = vcnl4040.getProximity();

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_vcnl4040_Chart, ui_vcnl4040_Chart_Series, raw);
        lv_chart_refresh(ui_vcnl4040_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_vcnl4040_Label, "Prx %d", raw);
        xSemaphoreGive(lvglSemaphore);
      }

      next_vcnl4040_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
