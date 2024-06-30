#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  pin_t VCC;
  pin_t D0;
  pin_t A0;
  pin_t GND;
  uint32_t intensity;
} chip_data_t;

bool is_powered(void *data) {
  chip_data_t *chip_data = (chip_data_t*)data;
// implement better check
   return pin_read(chip_data->VCC) && !pin_read(chip_data->GND);
}

static void chip_timer_callback(void *data);

void chip_init() {
  chip_data_t *chip_data = malloc(sizeof(chip_data_t));

  chip_data->VCC = pin_init("VCC", INPUT);
  chip_data->D0 = pin_init("D0", OUTPUT_LOW);
  chip_data->A0 = pin_init("A0", ANALOG);
  chip_data->GND = pin_init("GND", INPUT);
  
  chip_data->intensity = attr_init_float("intensity", 0);

  const timer_config_t timer_config = {
    .callback = chip_timer_callback,
    .user_data = chip_data
  };

  timer_t timer_id = timer_init(&timer_config);
  timer_start(timer_id, 1000, true);
}

void chip_timer_callback(void *data) {
  if (is_powered(data)) {
    chip_data_t *chip_data = (chip_data_t*)data;
    float intensity = attr_read_float(chip_data->intensity);
    pin_dac_write(chip_data->A0, intensity);
  }
}
