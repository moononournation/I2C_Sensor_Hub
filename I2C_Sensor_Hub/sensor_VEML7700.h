#pragma once

#include <Adafruit_VEML7700.h>

Adafruit_VEML7700 veml7700 = Adafruit_VEML7700();
bool veml7700_inited = false;
unsigned long next_veml7700_readings_ms = 0;
lv_obj_t *ui_veml7700_Chart;
lv_chart_series_t *ui_veml7700_Chart_Series;
lv_obj_t *ui_veml7700_box;
lv_obj_t *ui_veml7700_Label;
lv_color_t veml7700_color = lv_palette_main(LV_PALETTE_CYAN);

void update_veml7700(uint8_t addr)
{
  if (!veml7700_inited)
  {
    if (!veml7700.begin())
    {
      Serial.println("Couldn't find VEML7700 sensor");
    }
    else
    {
      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_veml7700_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_veml7700_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_veml7700_box);
        lv_obj_set_style_bg_color(ui_veml7700_box, veml7700_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_veml7700_box, 1);

        ui_veml7700_Label = lv_label_create(ui_veml7700_box);
        lv_obj_set_size(ui_veml7700_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_veml7700_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_veml7700_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_veml7700_Label, "VEML7700");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: VEML7700 Ambient Light Sensor", addr);

        ui_veml7700_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_veml7700_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_veml7700_Chart);
        lv_obj_set_y(ui_veml7700_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_veml7700_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 4096);
        lv_chart_set_type(ui_veml7700_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_veml7700_Chart, CHART_POINT_COUNT);

        ui_veml7700_Chart_Series = lv_chart_add_series(ui_veml7700_Chart, veml7700_color, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        veml7700_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (veml7700_inited)
  {
    if (millis() > next_veml7700_readings_ms)
    {
      uint16_t lux = veml7700.readLux();

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_veml7700_Chart, ui_veml7700_Chart_Series, lux);
        lv_chart_refresh(ui_veml7700_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_veml7700_Label, "Lux %d", lux);
        xSemaphoreGive(lvglSemaphore);
      }

      next_veml7700_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
