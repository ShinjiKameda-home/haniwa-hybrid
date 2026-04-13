#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Define constants
const uint LED_RED   = 13;
const uint LED_GREEN = 14;
const uint LED_BLUE  = 15;
const uint SENSOR_VCC = 22;

void blink_led(uint gpio, int duration_sec) {
    // 1 cycle = 1 second (0.5s ON + 0.5s OFF)
    // Simply loop 'duration_sec' times to blink for that many seconds.
    for (int i = 0; i < duration_sec; i++) {
        gpio_put(gpio, 1); // ON
        sleep_ms(500);
        gpio_put(gpio, 0); // OFF
        sleep_ms(500);
    }
}

int main() {
    // Initialize all of the SDK
    stdio_init_all();

    // Initialize LED pins
    const uint leds[] = {LED_RED, LED_GREEN, LED_BLUE};
    for (uint led : leds) {
        gpio_init(led);
        gpio_set_dir(led, GPIO_OUT);
    }

    // Initialize Soil Moisture Sensor pins
    gpio_init(SENSOR_VCC);
    gpio_set_dir(SENSOR_VCC, GPIO_OUT);

    // Initialize Analog-to-Digital Converter
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    while(true){
        // VCC ON
        gpio_put(SENSOR_VCC, 1);
        sleep_ms(50);

        // Print data to the PC terminal
        uint16_t reading = adc_read();
        printf("Soil Moisture: %d\n", reading);

        // VCC OFF
        gpio_put(SENSOR_VCC, 0);

        // Blinking LEDs of RGB
        blink_led(LED_RED, 3);
        blink_led(LED_GREEN, 3);
        blink_led(LED_BLUE, 3);
    }

}