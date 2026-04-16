#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "config.hpp"
#include "haniwa_connector.hpp"

// Constants
static bool is_connected = false;

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

// Sent callback
static err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("Message reached the server! Closing connection.\n");
    tcp_close(tpcb); // After sending the message, we can close the connection
    return ERR_OK;
}

// Sending data Callback
static err_t connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err == ERR_OK) {
        const char *msg = "Hello from the Aviator's Box (C++).";
        tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        printf("Transmission initiated: Waiting for server ACK.\n");
   } else {
        printf("Error: Connection callback failed (code: %d).\n", err);
        tcp_close(tpcb);
    }
    return ERR_OK;
}

// Say hello to the HomeServer
void haniwa_connector_send_hello() {
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

    printf("Connecting to server at %s:%d...\n", SERVER_IP, SERVER_PORT);

    // 3. Start connecting to the server
    // When the connection is complete, connected_callback will be called
    err_t err = tcp_connect(pcb, (ip_addr_t *)&server_addr, SERVER_PORT, connected_callback);
    
    if (err != ERR_OK) {
        printf("Error: tcp_connect initiation failed (code: %d).\n", err);
    }
}
