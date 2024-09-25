#pragma once

#include <SensorPCF85063.hpp>

SensorPCF85063 pcf85063;
bool pcf85063_inited = false;
unsigned long next_pcf85063_readings_ms = 0;
lv_obj_t *ui_pcf85063_box;
lv_obj_t *ui_pcf85063_Label;
lv_color_t pcf85063_color = lv_palette_main(LV_PALETTE_GREY);

void update_pcf85063(uint8_t addr)
{
  if (!pcf85063_inited)
  {
    if (!pcf85063.begin(Wire, addr, I2C_SDA, I2C_SCL))
    {
      Serial.println("Couldn't find PCF85063 sensor");
    }
    else
    {
      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        ui_pcf85063_box = lv_obj_create(ui_Flex);
        lv_obj_set_size(ui_pcf85063_box, LV_SIZE_CONTENT, (gfx->height() - LV_HEADER_SIZE - 54) / NUMBER_OF_ROW);
        lv_obj_center(ui_pcf85063_box);
        lv_obj_set_style_bg_color(ui_pcf85063_box, pcf85063_color, LV_PART_MAIN | LV_STATE_DEFAULT);
        // lv_obj_set_flex_grow(ui_pcf85063_box, 1);

        ui_pcf85063_Label = lv_label_create(ui_pcf85063_box);
        lv_obj_set_size(ui_pcf85063_Label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_pcf85063_Label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_font(ui_pcf85063_Label, LV_FONT_FIGURE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_pcf85063_Label, "PCF85063");

        pcf85063_inited = true;
        xSemaphoreGive(lvglSemaphore);
      }
    }
  }

  if (pcf85063_inited)
  {
    if (millis() > next_pcf85063_readings_ms)
    {
      RTC_DateTime datetime = pcf85063.getDateTime();

      if (xSemaphoreTake(lvglSemaphore, (TickType_t)10) == pdTRUE)
      {
        lv_label_set_text_fmt(ui_pcf85063_Label, "%04d-%02d-%02d  %02d:%02d:%02d", datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute, datetime.second);
        xSemaphoreGive(lvglSemaphore);
      }

      next_pcf85063_readings_ms += READINGS_MS_INTERVAL;
    }
  }
}
