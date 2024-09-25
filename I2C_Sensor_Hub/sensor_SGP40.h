#pragma once

#include <Adafruit_SGP40.h>

Adafruit_SGP40 sgp40;
bool sgp40_inited = false;
unsigned long next_sgp40_readings_ms = 0;
lv_obj_t *ui_sgp40_Chart;
lv_chart_series_t *ui_sgp40_Chart_Series;
lv_obj_t *ui_sgp40_box;
lv_obj_t *ui_sgp40_Label;
lv_color_t sgp40_color = lv_palette_main(LV_PALETTE_LIME);

void update_sgp40(uint8_t addr)
{
  if (!sgp40_inited)
  {
    sgp40 = Adafruit_SGP40();
    if (!sgp40.begin())
    {
      Serial.println("Couldn't find SGP40 sensor");
    }
    else
    {
      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_sgp40_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_sgp40_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_sgp40_box);
        lv_obj_set_style_bg_color(ui_sgp40_box, sgp40_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_sgp40_box, 1);

        ui_sgp40_Label = lv_label_create(ui_sgp40_box);
        lv_obj_set_size(ui_sgp40_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_sgp40_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_sgp40_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_sgp40_Label, "SGP40");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: SGP40 Air Quality Sensor", addr);

        ui_sgp40_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_sgp40_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_sgp40_Chart);
        lv_obj_set_y(ui_sgp40_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_sgp40_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 255);
        lv_chart_set_type(ui_sgp40_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_sgp40_Chart, CHART_POINT_COUNT);

        ui_sgp40_Chart_Series = lv_chart_add_series(ui_sgp40_Chart, sgp40_color, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        sgp40_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (sgp40_inited)
  {
    if (millis() > next_sgp40_readings_ms)
    {
      uint16_t voc = sgp40.measureVocIndex(); /* TODO: pinput temperature and humidity value */

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_sgp40_Chart, ui_sgp40_Chart_Series, voc);
        lv_chart_refresh(ui_sgp40_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_sgp40_Label, "VOCs %d", voc);
        xSemaphoreGive(lvglSemaphore);
      }

      next_sgp40_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
