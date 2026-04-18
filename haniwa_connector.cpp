#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "config.hpp"
#include "haniwa_main.hpp"
#include "haniwa_connector.hpp"

// Constants
static bool is_connected = false;
static uint16_t pending_moisture = 0;
static LEDStatus latest_status = STATUS_SKIP;
static bool result_received = false;

// Parse the result string to determine the LED status (internal function)
static LEDStatus parse_result_to_status(const char* data) {
    if (strstr(data, "RESULT:GO")) {
        return STATUS_GO;
    } else if (strstr(data, "RESULT:TOO_MUCH")) {
        return STATUS_TOO_MUCH;
    } 
    return STATUS_SKIP; 
}

// Sent callback
static err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("Message reached the server! Waiting for response.\n");
    return ERR_OK;
}

// Received callback
static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p != NULL) {
        static char tmp_buffer[32] = {0};
        // Copy the received data into a local buffer safely
        size_t len = (p->tot_len > 31) ? 31 : p->tot_len;
        pbuf_copy_partial(p, tmp_buffer, len, 0);
        tmp_buffer[len] = '\0';
        
        // Set the flag to indicate that a new result has been received
        result_received = true;

        // Parse the result and update the latest status
        latest_status = parse_result_to_status(tmp_buffer);
        printf("Haniwa: Received result from the HomeServer.\n");
        
        // Close the connection after processing the result immediately
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        tcp_close(tpcb);
    }
    return ERR_OK;
}

// Connected Callback
static err_t connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err == ERR_OK) {
        // register callbacks for receiving data and sent acknowledgments
        tcp_recv(tpcb, recv_callback);
        tcp_sent(tpcb, sent_callback);

        // Prepare the moisture data to send
        char msg[32];
        snprintf(msg, sizeof(msg), "MOISTURE:%u", pending_moisture);

        // Send the moisture data to the server
        tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);

        printf("Connection established. Reported: %s\n", msg);
   } else {
        printf("Error: Connection failed (code: %d).\n", err);
        tcp_close(tpcb);
    }
    return ERR_OK;
}

// Initialize
bool haniwa_connector_init() {
    if (cyw43_arch_init()) {
        printf("Error: Wi-Fi chip initialization failed.\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();

    const int max_retries = 3; // To acquire a brilliant military strategist...

    for (int i = 1; i <= max_retries; i++) {
        printf("Connection attempt %d/%d...\n", i, max_retries);
        
        // Attempt to connect with a 10-second timeout
        int state = cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000
        );

        if (state == 0) {
            printf("Connected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
            is_connected = true;
            return true;
        }

        if (i < max_retries) {
            printf("Connection failed. Retrying in 10 seconds...\n");
            sleep_ms(10000); 
        }
    }

    printf("Critical: Failed to connect after %d attempts.\n", max_retries);
    return false;
}

// Say hello to the HomeServer (connection logic)
void haniwa_send_hello() {
    // 1. Make a new TCP PCB (Protocol Control Block) which is like a "socket" in the TCP/IP world
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Error: Failed to allocate PCB.\n");
        return;
    }

    // 2. Exchange string format IP address to binary format
    ip4_addr_t server_addr;
    if (!ip4addr_aton(SERVER_IP, &server_addr)) {
        printf("Error: Invalid server IP address format.\n");
        return;
    }

    // 3. Start connecting to the server
    // When the connection is complete, connected_callback will be called
    err_t err = tcp_connect(pcb, (ip_addr_t *)&server_addr, SERVER_PORT, connected_callback);
    
    if (err != ERR_OK) {
        printf("Error: tcp_connect initiation failed (code: %d).\n", err);
        tcp_abort(pcb); // Release the PCB immediately on failure
    }
}

// Send moisture data to the HomeServer
void haniwa_send_data(uint16_t moisture) {
    // Store the latest moisture value to be sent when connected
    pending_moisture = moisture; 

    // Reuse the connection logic to send data
    haniwa_send_hello(); 
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
}


