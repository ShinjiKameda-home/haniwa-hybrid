#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "haniwa_main.hpp"
#include "haniwa_monitor.hpp"
#include "haniwa_connector.hpp"
#include "pico/cyw43_arch.h"

// Global Constants
const uint32_t REPORT_INTERVAL_MS = 15 * 60 * 1000; // Report moisture every 15 minutes
const uint32_t FLASH_INTERVAL_MS = 20 * 1000; // Flash LEDs every 20 seconds
uint32_t last_report_time = 0;
uint32_t last_flash_time = 0;
uint32_t flashing_start_time = 0;
bool flashing_led_state = false;
uint32_t loop_count = 0;
bool heartbeat_led_state = false;

void update_led_status(LEDStatus status) {
    // Turn off all LEDs before updating the status
    turn_off_all_leds();

    switch(status) {
        case STATUS_GO:
            start_led_blink(true, false, false, 500, 10); // Blink Red for GO watering
            printf("Haniwa: Red Alert! GO Watering!\n");
            break;
            
        case STATUS_TOO_MUCH:
            start_led_blink(false, false, true, 500, 10); // Blink Blue for TOO_MUCH water
            printf("Haniwa: Blue Alert! TOO_MUCH water.\n");
            break;

        case STATUS_ERROR:
            start_led_blink(false, false, true, 100, 50); // Fast Blink Blue for ERROR
            printf("Haniwa: Blue Alert!! ERROR occurred.\n");
            break;

        case STATUS_SKIP:
        default:
            start_led_blink(false, true, false, 500, 10); // Blink Green for SKIP water
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
    sleep_ms(100);

    // Initialize software modules
    haniwa_monitor_init();
    if (!haniwa_connector_init()) {
        printf("Haniwa Connector: The meeting has been rejected.\n");
        return -1; 
    }

    // Initialize the LED status to SKIP (Green) at the start
    LEDStatus current_status = STATUS_SKIP;

    // Initialize Wi-Fi and TCP connection to the HomeServer
    for (int i = 0; i < 5; i++) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);  // Turn on the Wi-Fi LED to indicate connection attempt
        sleep_ms(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);  // Turn off the Wi-Fi LED
        sleep_ms(100);
    }

    // Test the LED blinking
    haniwa_indication_on(STATUS_TOO_MUCH);
    sleep_ms(200);
    haniwa_flash_off();
    sleep_ms(200);
    haniwa_indication_on(STATUS_SKIP);
    sleep_ms(200);
    haniwa_flash_off();
    sleep_ms(200);
    haniwa_indication_on(STATUS_GO);
    sleep_ms(200);
    haniwa_flash_off();
    sleep_ms(200);
    haniwa_flash_on();
    sleep_ms(500);
    haniwa_flash_off();
    
    // Test sensor, and send the first moisture value to the HomeServer
    uint16_t val = get_moisture();
    printf("Current moisture: %u\n", val);
    haniwa_send_data(val);
    last_report_time = to_ms_since_boot(get_absolute_time());
    last_flash_time = to_ms_since_boot(get_absolute_time());
    
    // Enable the watchdog with an 8-second timeout to recover from potential hangs or bus errors
    watchdog_enable(8000, 1);

    // Main loop
    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        watchdog_update(); // Feed the watchdog to prevent reset

        // Polling LED blinking
        poll_led_blink();

        // Listen for incoming messages and handle Wi-Fi events
        haniwa_poll_result();

        // Let LED blinking to inform the person the result when the "Person" has come to the garden
        if (haniwa_recv_result(&current_status)) {
            update_led_status(current_status); 
            printf("Haniwa: Updated LED status based on the decision.\n");
        }

        // Flash LEDs to indicate keeping alive
        if (!flashing_led_state) {
            if (current_time - last_flash_time >= FLASH_INTERVAL_MS) {
                last_flash_time = current_time;
                flashing_start_time = current_time;
                flashing_led_state = true;
                // haniwa_flash_on(); // Flash all LEDs was cancelled instead of indicating the current status
                haniwa_indication_on(current_status);                
            }
        } else {
            if (current_time - flashing_start_time >= 500) {
                flashing_led_state = false;
                haniwa_flash_off();
            }
        }        

        // Check if it's time to report moisture to the HomeServer
        if (current_time - last_report_time >= REPORT_INTERVAL_MS) {
            last_report_time = current_time;
            val = get_moisture();
            printf("Current moisture: %u\n", val);
            haniwa_send_data(val);
        }

        // Heartbeat LED to indicate the system is alive
        loop_count++;
        if (loop_count >= 100) {
            loop_count = 0;
            heartbeat_led_state = !heartbeat_led_state;
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, heartbeat_led_state);
        }

        // Take interval for preventing CPU overheat and unnecessary network traffic
        sleep_ms(10);
    }

    return 0;
}