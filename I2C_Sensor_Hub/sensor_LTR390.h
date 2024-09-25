#pragma once

#include <Adafruit_LTR390.h>

Adafruit_LTR390 ltr390;
bool ltr390_inited = false;
unsigned long next_ltr390_readings_ms = 0;
lv_obj_t *ui_ltr390_Chart;
lv_chart_series_t *ui_ltr390_Chart_Series;
lv_obj_t *ui_ltr390_box;
lv_obj_t *ui_ltr390_Label;
lv_color_t ltr390_color = lv_palette_main(LV_PALETTE_PURPLE);

void update_ltr390(uint8_t addr)
{
  if (!ltr390_inited)
  {
    ltr390 = Adafruit_LTR390();
    if (!ltr390.begin())
    {
      Serial.println("Couldn't find LTR390 sensor");
    }
    else
    {
      ltr390.setMode(LTR390_MODE_UVS);
      ltr390.setGain(LTR390_GAIN_3);
      ltr390.setResolution(LTR390_RESOLUTION_16BIT);
      ltr390.setThresholds(100, 1000);
      ltr390.configInterrupt(false, LTR390_MODE_UVS);

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_ltr390_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_ltr390_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_ltr390_box);
        lv_obj_set_style_bg_color(ui_ltr390_box, ltr390_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_ltr390_box, 1);

        ui_ltr390_Label = lv_label_create(ui_ltr390_box);
        lv_obj_set_size(ui_ltr390_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_ltr390_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_ltr390_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_ltr390_Label, "LTR390");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: LTR390 ALS+UV Sensor", addr);

        ui_ltr390_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_ltr390_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_ltr390_Chart);
        lv_obj_set_y(ui_ltr390_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_ltr390_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 2000);
        lv_chart_set_type(ui_ltr390_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_ltr390_Chart, CHART_POINT_COUNT);

        ui_ltr390_Chart_Series = lv_chart_add_series(ui_ltr390_Chart, ltr390_color, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        ltr390_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (ltr390_inited)
  {
    if (millis() > next_ltr390_readings_ms)
    {
      if (ltr390.newDataAvailable())
      {
        uint32_t uvs = ltr390.readUVS();

        if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
        {
          lv_chart_set_next_value(ui_ltr390_Chart, ui_ltr390_Chart_Series, uvs);
          lv_chart_refresh(ui_ltr390_Chart); /*Required after direct set*/
          lv_label_set_text_fmt(ui_ltr390_Label, "UV %d", uvs);
          xSemaphoreGive(lvglSemaphore);
        }

        next_ltr390_readings_ms += READINGS_MS_INTERVAL;
      }
    }
  }
}
