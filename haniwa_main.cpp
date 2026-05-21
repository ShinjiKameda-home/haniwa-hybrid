#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "haniwa_main.hpp"
#include "haniwa_monitor.hpp"
#include "haniwa_connector.hpp"
#include "pico/cyw43_arch.h"

// Global Constants
const uint32_t REPORT_INTERVAL_MS = 15 * 60 * 1000; // Report moisture every 15 minutes
uint32_t last_report_time = 0;
uint32_t loop_count = 0;
bool heartbeat_led_state = false;

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

        case STATUS_ERROR:
            haniwa_led_hf_blue(10);
            printf("Haniwa: Blue Alert!! ERROR occurred.\n");
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
    if (watchdog_caused_reboot()) {
        printf("Haniwa: Rebooted by Watchdog! (Bus error or Hangup recovered)\n");
    }
    sleep_ms(500);

    // Initialize software modules
    haniwa_monitor_init();
    if (!haniwa_connector_init()) {
        printf("Haniwa Connector: The meeting has been rejected.\n");
        return -1; 
    }
    
    // Test LEDs and sensor, and send the first moisture value to the HomeServer
    haniwa_led_blink_red(1);
    haniwa_led_blink_green(1);
    haniwa_led_blink_blue(1);
    haniwa_led_hf_blue(1);
    uint16_t val = haniwa_get_moisture();
    printf("Current moisture: %u\n", val);
    haniwa_send_data(val);
    last_report_time = to_ms_since_boot(get_absolute_time());
    
    // Initialize the LED status to SKIP (Green) at the start
    LEDStatus current_status = STATUS_SKIP;
    
    // Enable the watchdog with an 8-second timeout to recover from potential hangs or bus errors
    watchdog_enable(8000, 1);

    // Main loop
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        watchdog_update(); // Feed the watchdog to prevent reset

        // Listen for incoming messages and handle Wi-Fi events
        haniwa_poll_result();

        // Let LED blinking to inform the person the result when the "Person" has come to the garden
        if (haniwa_recv_result(&current_status)) {
            update_led_status(current_status); 
            printf("Haniwa: Updated LED status based on the decision.\n");
        }

        // Check if it's time to report moisture to the HomeServer
        if (current_time - last_report_time >= REPORT_INTERVAL_MS) {
            // Update the last report time
            last_report_time = current_time;

            // Get moisture value from the sensor
            val = haniwa_get_moisture();

            // Print to the PC Terminal
            printf("Current moisture: %u\n", val);

            // Send moisture value to the HomeServer
            haniwa_send_data(val);
        }

        // Heartbeat LED to indicate the system is alive
        loop_count++;
        if (loop_count >= 2) {
            loop_count = 0;
            heartbeat_led_state = !heartbeat_led_state;
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, heartbeat_led_state);
        }

        // Take interval for preventing CPU overheat and unnecessary network traffic
        sleep_ms(500);
    }

    return 0;
}