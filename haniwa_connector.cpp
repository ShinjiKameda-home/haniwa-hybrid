/*
#include <stdio.h>
#include "pico/stdlib.h"
#include "config.hpp"
#include "haniwa_connector.hpp"
#include "pico/cyw43_arch.h"

// Constants


static err_t connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err == ERR_OK) {
        const char *msg = "Hello from the Aviator's Box (C++).";
        tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb); // パケットを射出！
        printf("Message sent to the center!\n");
    }
    return ERR_OK;
}

void run_hello_world() {
    // 1. Wi-Fiチップの初期化
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return;
    }

    cyw43_arch_enable_sta_mode();

    // 2. ネットワークの絆を結ぶ
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Failed to connect.\n");
        return;
    }
    printf("Connected! IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_default)));

    // 3. TCPプロトコルという「箱」を組み立てる
    struct tcp_pcb *pcb = tcp_new();
    ip_addr_t server_addr;
    ip4addr_aton(SERVER_IP, &server_addr);

    tcp_connect(pcb, &server_addr, SERVER_PORT, connected_callback);
}*/