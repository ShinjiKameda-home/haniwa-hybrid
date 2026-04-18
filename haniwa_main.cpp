#include <stdio.h>
#include "pico/stdlib.h"
#include "haniwa_main.hpp"
#include "haniwa_monitor.hpp"
#include "haniwa_connector.hpp"

// Constants
const uint32_t REPORT_INTERVAL_MS = 15 * 60 * 1000; // Report moisture every 15 minutes
uint32_t last_report_time = 0;
LEDStatus current_status = STATUS_SKIP;

void update_led_status(LEDStatus status) {
    // Turn off all LEDs before updating the status
    turn_off_all_leds();

    switch(status) {
        case STATUS_GO:
            haniwa_led_blink_red(10);
            printf("Haniwa: Red Alert! GO Watering!\n");
            break;
            
        case STATUS_TOO_MUCH:
            haniwa_led_blink_blue(10);
            printf("Haniwa: Blue Alert! TOO_MUCH water.\n");
            break;

        case STATUS_SKIP:
        default:
            haniwa_led_blink_green(10);
            printf("Haniwa: All Green. SKIP Watering.\n");
            break;
    }
}

// Main loop
int main() {
    // Initialize hardware itself
    stdio_init_all();

    // Initialize software modules
    haniwa_monitor_init();
    if (!haniwa_connector_init()) {
        printf("Haniwa Connector: The meeting has been rejected.\n");
        return -1; 
    }

    // Test Initialization
    uint16_t val = haniwa_get_moisture();
    haniwa_send_data(val);
    haniwa_led_blink_red(1);
    haniwa_led_blink_green(1);
    haniwa_led_blink_blue(1);
    sleep_ms(500);

    // Main loop
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());

        // Listen for incoming messages and handle Wi-Fi events
        haniwa_poll_result();

        // Let LED blinking to inform the person the result when the "Person" has come to the garden
        if (haniwa_recv_result(&current_status)) {
            update_led_status(current_status); 
            printf("Haniwa: Updated LED status based on the result.\n");
        }

        // Check if it's time to report moisture to the HomeServer
        if (current_time - last_report_time >= REPORT_INTERVAL_MS) {
            // Update the last report time
            last_report_time = current_time;

            // Get moisture value from the sensor
            val = haniwa_get_moisture();

            // Print to the PC Terminal for debug
            printf("Current moisture: %u\n", val);

            // Send moisture value to the HomeServer
            haniwa_send_data(val);
        }

        // Take interval for preventing CPU overheat and unnecessary network traffic
        sleep_ms(200);
    }

    return 0;
}