#include <stdio.h>
#include "haniwa_monitor.hpp"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Define pin numbers,
//  depending on your microcontroller board
const uint LED_SOURCE  = 17;
const uint LED_RED     = 13;
const uint LED_GREEN   = 14;
const uint LED_BLUE    = 15;
const uint SENSOR_VCC  = 22;

// GP26 is ADC0, GP27 is ADC1, and GP28 is ADC2, 
//  depending on your microcontroller board
const uint SENSOR_ADC_NUM = 0;
const uint SENSOR_AOUT = 26 + SENSOR_ADC_NUM;

// Additional variables
static bool     p_target_r = false;
static bool     p_target_g = false;
static bool     p_target_b = false;
static uint32_t p_interval_ms = 0;
static int      p_remaining_counts = 0;
static uint32_t p_last_toggle_time = 0;
static bool     p_led_is_on = false;
static bool     p_is_running = false;

// Turn off all LEDs
void turn_off_all_leds(void) {
    gpio_put(LED_SOURCE, 0);
    gpio_put(LED_RED, 1);
    gpio_put(LED_GREEN, 1);
    gpio_put(LED_BLUE, 1);
}

// Ready LEDs by turning on the LED source pin and waiting for a short time to stabilize
void ready_all_leds(void) {
    gpio_put(LED_SOURCE, 1);
    sleep_ms(20);
}

// Initialize hardware and ADC for soil moisture sensor
void haniwa_monitor_init() {
    // Initialize LED source pin
    gpio_init(LED_SOURCE);
    gpio_set_dir(LED_SOURCE, GPIO_OUT);
    gpio_put(LED_SOURCE, 0); // Start with all LEDs off (assuming active low)
    // Initialize LED pins
    const uint leds[] = {LED_RED, LED_GREEN, LED_BLUE};
    for (uint led : leds) {
        gpio_init(led);
        gpio_set_dir(led, GPIO_OUT);
        gpio_put(led, 1); // Start with all LEDs off (assuming active low)
    }

    // Initialize Soil Moisture Sensor pins
    gpio_init(SENSOR_VCC);
    gpio_set_dir(SENSOR_VCC, GPIO_OUT);

    // Initialize Analog-to-Digital Converter
    adc_init();
    adc_gpio_init(SENSOR_AOUT);
    adc_select_input(SENSOR_ADC_NUM);
}

// Start blinking LEDs with the specified colors, interval, and count
void start_led_blink(bool r, bool g, bool b, uint32_t interval_ms, int count) {
    turn_off_all_leds();

    p_target_r = r;
    p_target_g = g;
    p_target_b = b;
    p_interval_ms = interval_ms;
    p_remaining_counts = count * 2; // Each cycle has an ON and OFF state

    p_last_toggle_time = to_ms_since_boot(get_absolute_time());    
    p_led_is_on = false; // Start with LEDs off
    p_is_running = true;

    // Start with the first toggle immediately to turn on the LEDs
    ready_all_leds();
    if (p_target_r) gpio_put(LED_RED, 0);
    if (p_target_g) gpio_put(LED_GREEN, 0);
    if (p_target_b) gpio_put(LED_BLUE, 0);
    p_led_is_on = true;
    p_remaining_counts--;
}

// Monitor time called from the main loop to handle LED blinking
void poll_led_blink(void) {
    if (!p_is_running) return;

    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (now - p_last_toggle_time >= p_interval_ms) {
        p_last_toggle_time = now;

        if (p_remaining_counts <= 0) {
            turn_off_all_leds();
            p_is_running = false;
            return;
        }

        if (p_led_is_on) {
            // Turn off LEDs
            if (p_target_r) gpio_put(LED_RED, 1);
            if (p_target_g) gpio_put(LED_GREEN, 1);
            if (p_target_b) gpio_put(LED_BLUE, 1);
            p_led_is_on = false;
        } else {
            // Turn on LEDs
            ready_all_leds();
            if (p_target_r) gpio_put(LED_RED, 0);
            if (p_target_g) gpio_put(LED_GREEN, 0);
            if (p_target_b) gpio_put(LED_BLUE, 0);
            p_led_is_on = true;
        }

        p_remaining_counts--;
    }
}

// Get soil moisture value from the sensor
uint16_t get_moisture() {
        // VCC ON wait stabilizing sensor
        gpio_put(SENSOR_VCC, 1);
        sleep_ms(20);

        // Read soil moisture result
        uint16_t rslt = adc_read();
        watchdog_update(); // Feed the watchdog to prevent reset

        // VCC OFF immediately!
        gpio_put(SENSOR_VCC, 0);

        return rslt;
}

// Turn on all LEDs
void haniwa_flash_on(void) {
    ready_all_leds();
    gpio_put(LED_RED, 0);   // ON (Active Low)
    gpio_put(LED_GREEN, 0); // ON
    gpio_put(LED_BLUE, 0);  // ON
}

// Turn on an LED based on the current status
void haniwa_indication_on(LEDStatus status) {
    ready_all_leds();
    switch(status) {
        case STATUS_GO:
            gpio_put(LED_RED, 0);   // ON (Active Low)
            break;
        case STATUS_TOO_MUCH:
            gpio_put(LED_BLUE, 0);  // ON
            break;
        case STATUS_ERROR:
            gpio_put(LED_RED, 0);   // ON (Active Low)
            gpio_put(LED_GREEN, 0); // ON
            break;
        case STATUS_SKIP:
        default:
            gpio_put(LED_GREEN, 0); // ON
            break;
    }
}

// Turn off all LEDs
void haniwa_flash_off(void) {
    gpio_put(LED_RED, 1);   // OFF
    gpio_put(LED_GREEN, 1); // OFF
    gpio_put(LED_BLUE, 1);  // OFF
    turn_off_all_leds();
}
