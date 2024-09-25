#pragma once

#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>

Adafruit_TSL2591 tsl2591;
bool tsl2591_inited = false;
unsigned long next_tsl2591_readings_ms = 0;
lv_obj_t *ui_tsl2591_Chart;
lv_chart_series_t *ui_tsl2591_Chart_Series;
lv_obj_t *ui_tsl2591_box;
lv_obj_t *ui_tsl2591_Label;
lv_color_t tsl2591_color = lv_palette_main(LV_PALETTE_ORANGE);

void update_tsl2591(uint8_t addr)
{
  if (!tsl2591_inited)
  {
    tsl2591 = Adafruit_TSL2591(2591);
    if (!tsl2591.begin())
    {
      Serial.println("Couldn't find TSL2591 sensor");
    }
    else
    {
      // You can change the gain on the fly, to adapt to brighter/dimmer light situations
      // tsl2591.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
      tsl2591.setGain(TSL2591_GAIN_MED); // 25x gain
      // tsl2591.setGain(TSL2591_GAIN_HIGH);   // 428x gain

      // Changing the integration time gives you a longer time over which to sense light
      // longer timelines are slower, but are good in very low light situtations!
      // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
      // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_200MS);
      tsl2591.setTiming(TSL2591_INTEGRATIONTIME_300MS);
      // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_400MS);
      // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_500MS);
      // tsl2591.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_tsl2591_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_tsl2591_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_tsl2591_box);
        lv_obj_set_style_bg_color(ui_tsl2591_box, tsl2591_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_tsl2591_box, 1);

        ui_tsl2591_Label = lv_label_create(ui_tsl2591_box);
        lv_obj_set_size(ui_tsl2591_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_tsl2591_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_tsl2591_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_tsl2591_Label, "TSL2591");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: TSL2591 Light Sensor", addr);

        ui_tsl2591_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_tsl2591_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_tsl2591_Chart);
        lv_obj_set_y(ui_tsl2591_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_tsl2591_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 2000);
        lv_chart_set_type(ui_tsl2591_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_tsl2591_Chart, CHART_POINT_COUNT);

        ui_tsl2591_Chart_Series = lv_chart_add_series(ui_tsl2591_Chart, tsl2591_color, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        tsl2591_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (tsl2591_inited)
  {
    if (millis() > next_tsl2591_readings_ms)
    {
      uint16_t lux = tsl2591.getLuminosity(TSL2591_VISIBLE);

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_tsl2591_Chart, ui_tsl2591_Chart_Series, lux);
        lv_chart_refresh(ui_tsl2591_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_tsl2591_Label, "%d Lux", lux);
        xSemaphoreGive(lvglSemaphore);
      }

      next_tsl2591_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
