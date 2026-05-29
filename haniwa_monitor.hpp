#ifndef HANIWA_MONITOR_HPP
#define HANIWA_MONITOR_HPP

// Use C99 standard types
#include <stdint.h>

// Initialize
void haniwa_monitor_init(void);

// Blink LEDs
void start_led_blink(bool r, bool g, bool b, uint32_t interval_ms, int count);
void poll_led_blink(void);

// Turn off all LEDs
void turn_off_all_leds(void);

// Get soil moisture
uint16_t get_moisture(void);

// Flash all LEDs to indicate an error
void haniwa_flash_on(void);
void haniwa_flash_off(void);

#endif