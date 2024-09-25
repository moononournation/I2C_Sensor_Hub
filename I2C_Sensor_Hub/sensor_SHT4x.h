#pragma once

#include <Adafruit_SHT4x.h>

Adafruit_SHT4x sht4x;
bool sht4x_inited = false;
unsigned long next_sht4x_readings_ms = 0;
lv_obj_t *ui_sht4x_Chart;
lv_chart_series_t *ui_sht4x_Chart_Series1;
lv_chart_series_t *ui_sht4x_Chart_Series2;
lv_obj_t *ui_sht4x_box1;
lv_obj_t *ui_sht4x_box2;
lv_obj_t *ui_sht4x_Label1;
lv_obj_t *ui_sht4x_Label2;
lv_color_t sht4x_color1 = lv_palette_main(LV_PALETTE_RED);
lv_color_t sht4x_color2 = lv_palette_main(LV_PALETTE_BLUE);

void update_sht4x(uint8_t addr)
{
  if (!sht4x_inited)
  {
    sht4x = Adafruit_SHT4x();
    if (!sht4x.begin())
    {
      Serial.println("Couldn't find SHT4x sensor");
    }
    else
    {
      // You can have 3 different precisions, higher precision takes longer
      sht4x.setPrecision(SHT4X_HIGH_PRECISION);

      // You can have 6 different heater settings
      // higher heat and longer times uses more power
      // and reads will take longer too!
      sht4x.setHeater(SHT4X_NO_HEATER);

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_sht4x_box1 = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_sht4x_box1, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_sht4x_box1);
        lv_obj_set_style_bg_color(ui_sht4x_box1, sht4x_color1, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_sht4x_box1, 1);

        ui_sht4x_Label1 = lv_label_create(ui_sht4x_box1);
        lv_obj_set_size(ui_sht4x_Label1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_sht4x_Label1, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_sht4x_Label1, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_sht4x_Label1, "SHT4x");

        ui_sht4x_box2 = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_sht4x_box2, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_sht4x_box2);
        lv_obj_set_style_bg_color(ui_sht4x_box2, sht4x_color2, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_sht4x_box2, 1);

        ui_sht4x_Label2 = lv_label_create(ui_sht4x_box2);
        lv_obj_set_size(ui_sht4x_Label2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_sht4x_Label2, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_sht4x_Label2, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_sht4x_Label2, "SHT4x");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: SHT4x Temperature & Humidity Sensor", addr);

        ui_sht4x_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_sht4x_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_sht4x_Chart);
        lv_obj_set_y(ui_sht4x_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_sht4x_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
        lv_chart_set_type(ui_sht4x_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_sht4x_Chart, CHART_POINT_COUNT);

        ui_sht4x_Chart_Series1 = lv_chart_add_series(ui_sht4x_Chart, sht4x_color1, LV_CHART_AXIS_PRIMARY_Y);
        ui_sht4x_Chart_Series2 = lv_chart_add_series(ui_sht4x_Chart, sht4x_color2, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        sht4x_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (sht4x_inited)
  {
    if (millis() > next_sht4x_readings_ms)
    {
      sensors_event_t humidity, temp;
      sht4x.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_sht4x_Chart, ui_sht4x_Chart_Series1, temp.temperature);
        lv_chart_set_next_value(ui_sht4x_Chart, ui_sht4x_Chart_Series2, humidity.relative_humidity);
        lv_chart_refresh(ui_sht4x_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_sht4x_Label1, "%.1f°C", temp.temperature);
        lv_label_set_text_fmt(ui_sht4x_Label2, "%.1f%%", humidity.relative_humidity);
        xSemaphoreGive(lvglSemaphore);
      }

      next_sht4x_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
