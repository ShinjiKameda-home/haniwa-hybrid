#include <stdio.h>
#include "pico/stdlib.h"
#include "haniwa_monitor.hpp"
#include "haniwa_connector.hpp"

int main() {
    // Initialize hardware itself
    stdio_init_all();

    // Initialize modules
    haniwa_monitor_init();
    if (!haniwa_connector_init()) {
        printf("Haniwa Connector: The meeting has been rejected.\n");
        return -1; 
    }

    // Test Initialization
    haniwa_led_blink_red(1);
    haniwa_led_blink_green(1);
    haniwa_led_blink_blue(1);

    sleep_ms(2000);

    haniwa_connector_send_hello();

    // Main loop
    while (true) {
        for (int i = 0; i < 500; i++) {
            cyw43_arch_poll(); // こまめに耳を澄ませる
            sleep_ms(10);      // ちょっとだけ休憩
        }

        // Get moisture value from the sensor
        uint16_t val = haniwa_get_moisture();

        // Print to the PC Terminal for debug
        printf("Current moisture: %u\n", val);
    }

    return 0;
}