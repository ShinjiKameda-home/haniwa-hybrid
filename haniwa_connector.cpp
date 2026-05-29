#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/watchdog.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
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

// Setup static IP address for the Pico (HANIWA)
void setup_static_ip() {
    ip_addr_t ip, mask, gw, dns_server;
    ipaddr_aton(STATIC_IP, &ip);
    ipaddr_aton(NETMASK, &mask);
    ipaddr_aton(GATEWAY, &gw);
    ipaddr_aton(DNS_SERVER, &dns_server);
    // Configure the netif with the static IP settings
    struct netif *netif = &cyw43_state.netif[CYW43_ITF_STA];
    // Stop DHCP
    #if LWIP_DHCP
    dhcp_stop(netif);
    #endif
    // Set static IP configuration
    netif_set_addr(netif, &ip, &mask, &gw);
    // Optionally set DNS server if your application uses it
    dns_setserver(0, (const ip_addr_t *)&dns_server);
    printf("[HaniwaConnector] Static IP applied: %s\n", STATIC_IP);
}


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
        // snprintf(msg, sizeof(msg), "MOISTURE:%u\n", pending_moisture);
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

    // Register error callback to handle connection errors
    tcp_err(haniwa_pcb, error_callback); 

    // 2. Exchange string format IP address to binary format
    ip4_addr_t server_addr;
    if (!ip4addr_aton(SERVER_IP, &server_addr)) {
        printf("Error: Invalid server IP address format.\n");
        haniwa_pcb = NULL;
        is_connected = false;
        return false;
    }

    // Mark as not connected until the connection is established in the callback
    is_connected = false;

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

    // Wait 10 seconds for the connection to be established in the callback
    printf("Waiting for TCP handshake to complete...\n");
    for (int timeout = 0; timeout < 500; timeout++) {
        cyw43_arch_poll();
        watchdog_update();
        if (is_connected) {
            return true; // Connection really established successfully
        }
        sleep_ms(20);
    }

    // Timeout
    printf("Error: TCP handshake timed out.\n");
    if (haniwa_pcb) {
        tcp_abort(haniwa_pcb); // Abort the PCB to clean up resources
    }
    haniwa_pcb = NULL;
    is_connected = false;
    return false;
}

// Initialize
bool haniwa_connector_init() {
    if (check_connection())
        return true; // Already connected, no need to initialize again

    if (haniwa_pcb) {
        tcp_abort(haniwa_pcb); // Abort any existing PCB to ensure a clean state
        haniwa_pcb = NULL;
    }

    static bool is_hw_initialized = false;
    if (!is_hw_initialized) {
        if (cyw43_arch_init()) {
            watchdog_update();
            printf("Error: Wi-Fi chip initialization failed.\n");
            return false;
        }
        cyw43_arch_enable_sta_mode();
        cyw43_wifi_pm(&cyw43_state, cyw43_pm_value(CYW43_PM2_POWERSAVE_MODE, 200, 1, 1, 10));
        setup_static_ip(); // Apply static IP configuration
        is_hw_initialized = true;
    }

    watchdog_update();
    const int max_retries = 20;

    for (int i = 1; i <= max_retries; i++) {
        printf("Connection attempt %d/%d...\n", i, max_retries);
        watchdog_update();

        // Check connection to the rooter
        int link_status = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
        bool wifi_ready = (link_status == CYW43_LINK_UP);

        // If the link status is negative, it indicates a hardware error
        if (!wifi_ready) {
            printf("Connecting to Wi-Fi AP...\n");
            int state = cyw43_arch_wifi_connect_timeout_ms(
                WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 5000
            );
            watchdog_update();

            // retry if connection failed after 10 seconds, feeding the watchdog
            if (state == 0) {
                wifi_ready = true; 
            } else {
                printf("Wi-Fi connection failed with status: %d\n", state);
                if (i < max_retries) {
                    printf("Retrying in 10 seconds...\n");
                    for (int j = 0; j < 10; j++) {
                        sleep_ms(1000); 
                        watchdog_update();
                    }
                }
            }
        }

        // Wi-Fi is ready, now try to connect to the HomeServer
        if (wifi_ready) {
            if (connect_to_server()) {
                watchdog_update();
                printf("Connected. IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));
                is_connected = true;
                return true;
            } else {
                printf("Wi-Fi is OK, but TCP to HomeServer failed.\n");
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
        if (!haniwa_connector_init()) {
            printf("Haniwa: Reconnection failed. Will retry later.\n");
            return; // If reconnection fails, just return and wait for the next attempt
        }
        printf("Haniwa: Reconnected successfully. Sending data...\n");
        // After reconnection, we can proceed to send the data below without needing to prepare it again
    }

    // Store the latest moisture value to be sent when connected
    pending_moisture = moisture; 

    // Prepare the moisture data to send
    char msg[32];
    snprintf(msg, sizeof(msg), "MOISTURE:%u\n", pending_moisture);

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

    // Check physical connection status and attempt to reconnect if necessary
    int wifi_status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);

    if (wifi_status < 0) {
        // printf("Wi-Fi link status error: %d\n", wifi_status);
        // watchdog_enable(1, 1); // Reboot immediately on Wi-Fi hardware failure
        // while(1); // Wait for watchdog to reset the device
        if (is_connected) {
            printf("Warning: Wi-Fi dropped momentarily (status: %d). \n", wifi_status);
            is_connected = false;
        }
    }

    // If the TCP connection is only disconnected, try to soft-reset
    if (!is_connected) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_reconnect_attempt > RECONNECT_DELAY_MS) {
            last_reconnect_attempt = now;
            
            if (haniwa_pcb != NULL) {
                tcp_abort(haniwa_pcb); // Abort any existing PCB to ensure a clean state
                haniwa_pcb = NULL;     // Clear the PCB reference
            }

            is_connected = false; // Mark as not connected before attempting to reconnect

            printf("TCPConnection lost. Retrying to connect to HomeServer...\n");
            haniwa_connector_init();
        }
    }
}
