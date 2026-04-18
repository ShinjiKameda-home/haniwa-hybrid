#include <stdio.h>
#include "haniwa_monitor.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Define pin numbers,
//  depending on your microcontroller board
const uint LED_RED   = 13;
const uint LED_GREEN = 14;
const uint LED_BLUE  = 15;
const uint SENSOR_VCC = 22;

// GP26 is ADC0, GP27 is ADC1, and GP28 is ADC2, 
//  depending on your microcontroller board
const uint SENSOR_ADC_NUM = 0;
const uint SENSOR_AOUT = 26 + SENSOR_ADC_NUM;

void haniwa_monitor_init() {
    // Initialize LED pins
    const uint leds[] = {LED_RED, LED_GREEN, LED_BLUE};
    for (uint led : leds) {
        gpio_init(led);
        gpio_set_dir(led, GPIO_OUT);
        gpio_put(led, 0);
    }

    // Initialize Soil Moisture Sensor pins
    gpio_init(SENSOR_VCC);
    gpio_set_dir(SENSOR_VCC, GPIO_OUT);

    // Initialize Analog-to-Digital Converter
    adc_init();
    adc_gpio_init(SENSOR_AOUT);
    adc_select_input(SENSOR_ADC_NUM);
}

void haniwa_led_blink_red(int seconds) {
    for (int i = 0; i < seconds; i++) {
        gpio_put(LED_RED, 1);
        sleep_ms(500);
        gpio_put(LED_RED, 0);
        sleep_ms(500);
    }
}

void haniwa_led_blink_green(int seconds) {
    for (int i = 0; i < seconds; i++) {
        gpio_put(LED_GREEN, 1);
        sleep_ms(500);
        gpio_put(LED_GREEN, 0);
        sleep_ms(500);
    }
}

void haniwa_led_blink_blue(int seconds) {
    for (int i = 0; i < seconds; i++) {
        gpio_put(LED_BLUE, 1);
        sleep_ms(500);
        gpio_put(LED_BLUE, 0);
        sleep_ms(500);
    }
}

void turn_off_all_leds(void) {
    gpio_put(LED_RED, 0);
    gpio_put(LED_GREEN, 0);
    gpio_put(LED_BLUE, 0);
}

uint16_t haniwa_get_moisture() {
        // VCC ON wait stabilizing sensor
        gpio_put(SENSOR_VCC, 1);
        sleep_ms(50);

        // Read soil moisture result
        uint16_t rslt = adc_read();

        // VCC OFF immediately!
        gpio_put(SENSOR_VCC, 0);

        return rslt;
}
