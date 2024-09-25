#pragma once

#include <Adafruit_ADS1X15.h>

Adafruit_ADS1X15 ads1x15;
bool ads1x15_inited = false;
unsigned long next_ads1x15_readings_ms = 0;
lv_obj_t *ui_ads1x15_Chart;
lv_chart_series_t *ui_ads1x15_Chart_Series1;
lv_chart_series_t *ui_ads1x15_Chart_Series2;
lv_chart_series_t *ui_ads1x15_Chart_Series3;
lv_chart_series_t *ui_ads1x15_Chart_Series4;
lv_obj_t *ui_ads1x15_box1;
lv_obj_t *ui_ads1x15_box2;
lv_obj_t *ui_ads1x15_box3;
lv_obj_t *ui_ads1x15_box4;
lv_obj_t *ui_ads1x15_Label1;
lv_obj_t *ui_ads1x15_Label2;
lv_obj_t *ui_ads1x15_Label3;
lv_obj_t *ui_ads1x15_Label4;
lv_color_t ads1x15_color1 = lv_palette_main(LV_PALETTE_PINK);
lv_color_t ads1x15_color2 = lv_palette_main(LV_PALETTE_DEEP_PURPLE);
lv_color_t ads1x15_color3 = lv_palette_main(LV_PALETTE_LIGHT_BLUE);
lv_color_t ads1x15_color4 = lv_palette_main(LV_PALETTE_TEAL);

void update_ads1x15(uint8_t addr)
{
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads1x15.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads1x15.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads1x15.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads1x15.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads1x15.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads1x15.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads1x15_inited)
  {
    ads1x15 = Adafruit_ADS1X15();
    if (!ads1x15.begin())
    {
      Serial.println("Couldn't find ADS1X15 sensor");
    }
    else
    {
      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_ads1x15_box1 = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_ads1x15_box1, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_ads1x15_box1);
        lv_obj_set_style_bg_color(ui_ads1x15_box1, ads1x15_color1, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_ads1x15_box1, 1);

        ui_ads1x15_Label1 = lv_label_create(ui_ads1x15_box1);
        lv_obj_set_size(ui_ads1x15_Label1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_ads1x15_Label1, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_ads1x15_Label1, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_ads1x15_Label1, "ADS1X15");

        ui_ads1x15_box2 = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_ads1x15_box2, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_ads1x15_box2);
        lv_obj_set_style_bg_color(ui_ads1x15_box2, ads1x15_color2, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_ads1x15_box2, 1);

        ui_ads1x15_Label2 = lv_label_create(ui_ads1x15_box2);
        lv_obj_set_size(ui_ads1x15_Label2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_ads1x15_Label2, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_ads1x15_Label2, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_ads1x15_Label2, "ADS1X15");

        ui_ads1x15_box3 = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_ads1x15_box3, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_ads1x15_box3);
        lv_obj_set_style_bg_color(ui_ads1x15_box3, ads1x15_color3, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_ads1x15_box3, 1);

        ui_ads1x15_Label3 = lv_label_create(ui_ads1x15_box3);
        lv_obj_set_size(ui_ads1x15_Label3, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_ads1x15_Label3, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_ads1x15_Label3, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_ads1x15_Label3, "ADS1X15");

        ui_ads1x15_box4 = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_ads1x15_box4, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_ads1x15_box4);
        lv_obj_set_style_bg_color(ui_ads1x15_box4, ads1x15_color4, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_ads1x15_box4, 1);

        ui_ads1x15_Label4 = lv_label_create(ui_ads1x15_box4);
        lv_obj_set_size(ui_ads1x15_Label4, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_ads1x15_Label4, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_ads1x15_Label4, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_ads1x15_Label4, "ADS1X15");

        ui_Screen[screen_count] = lv_obj_create(NULL);
        lv_obj_clear_flag(ui_Screen[screen_count], LV_OBJ_FLAG_SCROLLABLE); /// Flags

        lv_obj_t *screen_label = lv_label_create(ui_Screen[screen_count]);
        lv_obj_set_size(screen_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(screen_label, LV_ALIGN_TOP_MID);
        lv_obj_set_y(screen_label, 4);
        lv_obj_set_style_text_font(screen_label, LV_FONT_HEADER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(screen_label, "0x%02x: ADS1X15 ADC Sensor", addr);

        ui_ads1x15_Chart = lv_chart_create(ui_Screen[screen_count]);
        lv_obj_set_size(ui_ads1x15_Chart, gfx->width() - 20, gfx->height() - LV_HEADER_SIZE);
        lv_obj_center(ui_ads1x15_Chart);
        lv_obj_set_y(ui_ads1x15_Chart, (LV_HEADER_SIZE - 20) / 2);
        lv_chart_set_range(ui_ads1x15_Chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
        lv_chart_set_type(ui_ads1x15_Chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
        lv_chart_set_point_count(ui_ads1x15_Chart, CHART_POINT_COUNT);

        ui_ads1x15_Chart_Series1 = lv_chart_add_series(ui_ads1x15_Chart, ads1x15_color1, LV_CHART_AXIS_PRIMARY_Y);
        ui_ads1x15_Chart_Series2 = lv_chart_add_series(ui_ads1x15_Chart, ads1x15_color2, LV_CHART_AXIS_PRIMARY_Y);
        ui_ads1x15_Chart_Series3 = lv_chart_add_series(ui_ads1x15_Chart, ads1x15_color3, LV_CHART_AXIS_PRIMARY_Y);
        ui_ads1x15_Chart_Series4 = lv_chart_add_series(ui_ads1x15_Chart, ads1x15_color4, LV_CHART_AXIS_PRIMARY_Y);

        ++screen_count;
        ads1x15_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (ads1x15_inited)
  {
    if (millis() > next_ads1x15_readings_ms)
    {
      int16_t adc0 = ads1x15.readADC_SingleEnded(0);
      int16_t adc1 = ads1x15.readADC_SingleEnded(1);
      int16_t adc2 = ads1x15.readADC_SingleEnded(2);
      int16_t adc3 = ads1x15.readADC_SingleEnded(3);

      float volts0 = ads1x15.computeVolts(adc0);
      float volts1 = ads1x15.computeVolts(adc1);
      float volts2 = ads1x15.computeVolts(adc2);
      float volts3 = ads1x15.computeVolts(adc3);

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_chart_set_next_value(ui_ads1x15_Chart, ui_ads1x15_Chart_Series1, volts0);
        lv_chart_set_next_value(ui_ads1x15_Chart, ui_ads1x15_Chart_Series2, volts1);
        lv_chart_set_next_value(ui_ads1x15_Chart, ui_ads1x15_Chart_Series3, volts2);
        lv_chart_set_next_value(ui_ads1x15_Chart, ui_ads1x15_Chart_Series4, volts3);
        lv_chart_refresh(ui_ads1x15_Chart); /*Required after direct set*/
        lv_label_set_text_fmt(ui_ads1x15_Label1, "A0 %.1fV", volts0);
        lv_label_set_text_fmt(ui_ads1x15_Label2, "A1 %.1fV", volts1);
        lv_label_set_text_fmt(ui_ads1x15_Label3, "A2 %.1fV", volts2);
        lv_label_set_text_fmt(ui_ads1x15_Label4, "A3 %.1fV", volts3);
        xSemaphoreGive(lvglSemaphore);
      }

      next_ads1x15_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
