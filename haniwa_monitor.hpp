#ifndef HANIWA_MONITOR_HPP
#define HANIWA_MONITOR_HPP

// Use C99 standard types
#include <stdint.h>

// Initialize
void haniwa_monitor_init(void);

// Blink LEDs
void haniwa_led_blink_red(int seconds);
void haniwa_led_blink_green(int seconds);
void haniwa_led_blink_blue(int seconds);

// Turn off all LEDs
void turn_off_all_leds(void);

// Get soil moisture
uint16_t haniwa_get_moisture(void);

#endif