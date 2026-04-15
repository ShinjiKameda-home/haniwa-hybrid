#include <stdio.h>
#include "pico/stdlib.h"
#include "haniwa_monitor.hpp"
// #include "haniwa_connector.hpp"

int main() {
    // Initialize hardware itself
    stdio_init_all();

    // Initialize modules
    haniwa_monitor_init();
    // haniwa_connector_init();

    // Test Initialization
    haniwa_led_blink_red(1);
    haniwa_led_blink_green(1);
    haniwa_led_blink_blue(1);

    sleep_ms(2000);

    // Main loop
    while (true) {
        uint16_t val = haniwa_get_moisture();

        // Print to the PC Terminal for debug
        printf("Current moisture: %u\n", val);
        
        sleep_ms(5000);
    }

    return 0;
}