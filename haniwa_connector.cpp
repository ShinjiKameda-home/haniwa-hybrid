#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/watchdog.h"
#include "lwip/tcp.h"
#include "config.hpp"
#include "haniwa_main.hpp"
#include "haniwa_connector.hpp"

// Constants
static bool is_connected = false;
static uint16_t pending_moisture = 0;
static LEDStatus latest_status = STATUS_SKIP;
static bool result_received = false;
static struct tcp_pcb *haniwa_pcb = NULL; // Global PCB for managing the connection
static uint32_t last_reconnect_attempt = 0;
const uint32_t RECONNECT_DELAY_MS = 10000; // 10 seconds

// Parse the result string to determine the LED status (internal function)
static LEDStatus parse_result_to_status(const char* data) {
    if (data == NULL || data[0] == '\0') {
        return STATUS_SKIP;
    }
    if (data[0] >= '0' && data[0] <= '3') {
        return (LEDStatus)(data[0] - '0');
    }
    return STATUS_SKIP; 
}

// Sent callback
static err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("Message reached the server!\n");
    return ERR_OK;
}

// Received callback
static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Server closed the connection
        printf("Connection closed by server.\n");

        is_connected = false;
        haniwa_pcb = NULL; // Clear the PCB reference

        tcp_arg(tpcb, NULL);
        tcp_sent(tpcb, NULL);
        tcp_recv(tpcb, NULL);
        tcp_close(tpcb);

        return ERR_OK;
    }

    if (err == ERR_OK && p->tot_len > 0) {
        static char tmp_buffer[32] = {0};
        size_t len = (p->tot_len > 31) ? 31 : p->tot_len;
        pbuf_copy_partial(p, tmp_buffer, len, 0);
        tmp_buffer[len] = '\0';
        
        result_received = true;
        latest_status = parse_result_to_status(tmp_buffer);
        printf("Received result from the HomeServer. Status: %d\n", latest_status);
        
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        // Not close the connection immediately to allow for potential further communication
        // tcp_close(tpcb);

    } else {
        // Error or empty packet, just free it
        pbuf_free(p);
    }
    return ERR_OK;
}

// Connected Callback
static err_t connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err == ERR_OK) {
        is_connected = true;
        haniwa_pcb = tpcb; // Store the PCB for future use
        printf("Successfully connected to the HomeServer.\n");
        // register callbacks for receiving data and sent acknowledgments
        tcp_recv(tpcb, recv_callback);
        tcp_sent(tpcb, sent_callback);
        // Send any pending moisture data immediately upon connection
        // char msg[32];
        // snprintf(msg, sizeof(msg), "MOISTURE:%u", pending_moisture);
        // tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        // tcp_output(tpcb);
    } else {
        printf("Error: Connection failed (code: %d).\n", err);
        tcp_close(tpcb);
        haniwa_pcb = NULL;
        is_connected = false;
    }
    return ERR_OK;
}

// Error Callback
static void error_callback(void *arg, err_t err) {
    printf("TCP connection error: %d\n", err);
    is_connected = false;
    haniwa_pcb = NULL; // Clear the PCB reference
}

// Check if the device is currently connected to the server
bool check_connection() {
    is_connected = (haniwa_pcb != NULL && haniwa_pcb->state == ESTABLISHED);
    return is_connected;    
}

// Connect to the HomeServer
bool connect_to_server() {
    // 1. Make a new TCP PCB (Protocol Control Block) which is like a "socket" in the TCP/IP world
    haniwa_pcb = tcp_new();
    if (!haniwa_pcb) {
        printf("Error: Failed to allocate PCB.\n");
        haniwa_pcb = NULL;
        is_connected = false;
        return false;
    }

    // 2. Exchange string format IP address to binary format
    ip4_addr_t server_addr;
    if (!ip4addr_aton(SERVER_IP, &server_addr)) {
        printf("Error: Invalid server IP address format.\n");
        haniwa_pcb = NULL;
        is_connected = false;
        return false;
    }

    // 3. Start connecting to the server
    // When the connection is complete, connected_callback will be called
    err_t err = tcp_connect(haniwa_pcb, (ip_addr_t *)&server_addr, SERVER_PORT, connected_callback);
    
    if (err != ERR_OK) {
        printf("Error: tcp_connect initiation failed (code: %d).\n", err);
        tcp_abort(haniwa_pcb); // Release the PCB immediately on failure
        haniwa_pcb = NULL;
        is_connected = false;
        return false;
    }

    // Connection initiation successful, now we wait for the connected_callback to be called
    return true;
}

// Initialize
bool haniwa_connector_init() {
    if (check_connection())
        return true; // Already connected, no need to initialize again

    if (haniwa_pcb) {
        tcp_abort(haniwa_pcb); // Abort any existing PCB to ensure a clean state
        haniwa_pcb = NULL;
    }

    if (cyw43_arch_init()) {
        printf("Error: Wi-Fi chip initialization failed.\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();
    cyw43_wifi_pm(&cyw43_state, cyw43_pm_value(CYW43_PM2_POWERSAVE_MODE, 200, 1, 1, 10));

    const int max_retries = 3; // To acquire a brilliant military strategist...

    for (int i = 1; i <= max_retries; i++) {
        printf("Connection attempt %d/%d...\n", i, max_retries);
        
        // Attempt to connect with a 10-second timeout
        int state = cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000
        );

        connect_to_server(); // Start the TCP connection process
        
        if (state == 0) {
            printf("Connected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
            is_connected = true;
            return true;
        }

        if (i < max_retries) {
            printf("Connection failed. Retrying in 10 seconds...\n");
            for (int j = 0; j < 10; j++) {
                sleep_ms(1000); 
                watchdog_update(); // Feed the watchdog to prevent reset during the wait
            }
        }
    }

    printf("Critical: Failed to connect after %d attempts.\n", max_retries);
    return false;
}

// Send moisture data to the HomeServer
void haniwa_send_data(uint16_t moisture) {
    // Try to reconnect if not connected
    if (!is_connected) {
        printf("Haniwa: Connection lost. Retrying...\n");
        haniwa_connector_init();
        return;
    }

    // Store the latest moisture value to be sent when connected
    pending_moisture = moisture; 

    // Prepare the moisture data to send
    char msg[32];
    snprintf(msg, sizeof(msg), "MOISTURE:%u", pending_moisture);

    // Send the moisture data to the server
    tcp_write(haniwa_pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
    tcp_output(haniwa_pcb);
}

// Check a new result from the HomeServer
bool haniwa_recv_result(LEDStatus* out_status) {
    if (result_received) {
        *out_status = latest_status;
        result_received = false; // reset the flag for the next result
        return true;
    }
    return false;
}

// Polling function to check for new results (can be called in the main loop)
void haniwa_poll_result() {
    cyw43_arch_poll(); // Listen for incoming messages and handle Wi-Fi events
    if (!is_connected) {
        if (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) < 0) {
            printf("Wi-Fi hardware looks dead. Need hard reset...\n");
        }

        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_reconnect_attempt > RECONNECT_DELAY_MS) {
            last_reconnect_attempt = now;
            if (haniwa_pcb != NULL) {
                tcp_abort(haniwa_pcb); // Abort any existing PCB to ensure a clean state
                haniwa_pcb = NULL;     // Clear the PCB reference
            }
            printf("Connection lost. Retrying to connect to HomeServer...\n");
            haniwa_connector_init(); 
        }
    }
}
