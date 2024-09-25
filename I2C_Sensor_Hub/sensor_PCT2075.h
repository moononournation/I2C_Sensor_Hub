#pragma once

#include <Adafruit_PCT2075.h>

Adafruit_PCT2075 pct2075;
bool pct2075_inited = false;
unsigned long next_pct2075_readings_ms = 0;
lv_obj_t *ui_pct2075_Chart;
lv_chart_series_t *ui_pct2075_Chart_Series;
lv_obj_t *ui_pct2075_box;
lv_obj_t *ui_pct2075_Label;
lv_color_t pct2075_color = lv_palette_main(LV_PALETTE_GREEN);

void update_pct2075(uint8_t addr)
{
  if (!pct2075_inited)
  {
    pct2075 = Adafruit_PCT2075();
    if (!pct2075.begin())
    {
      Serial.println("Couldn't find PCT2075 sensor");
    }
    else
    {
      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_pct2075_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_pct2075_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_pct2075_box);
        lv_obj_set_style_bg_color(ui_pct2075_box, pct2075_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_pct2075_box, 1);

        ui_pct2075_Label = lv_label_create(ui_pct2075_box);
        lv_obj_set_size(ui_pct2075_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_pct2075_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_pct2075_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_pct2075_Label, "PCT2075");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: PCT2075 Temperature Sensor", addr);

        ui_pct2075_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_pct2075_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_pct2075_Chart);
        lv_obj_set_y(ui_pct2075_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_pct2075_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 40);
        lv_chart_set_type(ui_pct2075_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_pct2075_Chart, CHART_POINT_COUNT);

        ui_pct2075_Chart_Series = lv_chart_add_series(ui_pct2075_Chart, pct2075_color, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        pct2075_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (pct2075_inited)
  {
    if (millis() > next_pct2075_readings_ms)
    {
      float temp = pct2075.getTemperature();

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_pct2075_Chart, ui_pct2075_Chart_Series, temp);
        lv_chart_refresh(ui_pct2075_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_pct2075_Label, "%.1f°C", temp);
        xSemaphoreGive(lvglSemaphore);
      }

      next_pct2075_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
