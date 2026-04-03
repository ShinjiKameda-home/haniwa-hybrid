#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "pico/cyw43_arch.h"

// UARTの設定（ハニワさんとの通信用）
#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// 土壌水分の閾値（キャリブレーション後に調整してね）
const int THRESHOLD_WET = 700;
const int THRESHOLD_MOIST = 400;

// ハニワさんのLED状態を更新する関数
void update_haniwa_led(int moisture_level) {
    char command[32];
    if (moisture_level > THRESHOLD_WET) {
        sprintf(command, "LED:BLUE\n");  // 潤い
    } else if (moisture_level > THRESHOLD_MOIST) {
        sprintf(command, "LED:GREEN\n"); // ちょうどいい
    } else {
        sprintf(command, "LED:RED\n");   // 乾き
    }
    uart_puts(UART_ID, command);
}

int main() {
    stdio_init_all();

    // 1. Wi-Fiチップの初期化
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    cyw43_arch_enable_sta_mode();

    // 2. UART初期化（ハニワさんとの絆）
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // 3. Wi-Fi接続（Dr. Wadachiたちと繋がる）
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms("YOUR_SSID", "YOUR_PASSWORD", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Failed to connect.\n");
        return -1;
    }

    // メインループ
    while (true) {
        // A. Dr. Wadachi（Cloud）からの問い合わせ待ち
        // ここでTCP/HTTPのリクエストをチェック
        // if (request_from_cloud) { 
        //    uart_puts(UART_ID, "GET:MOISTURE\n"); // ハニワに聞く
        //    send_response_to_cloud(data);
        // }

        // B. BirdWatcher（Edge）からの通知チェック
        // if (person_detected) {
           int current_moisture = 500; // 本来はUARTから取得した値
           update_haniwa_led(current_moisture);
        // }

        sleep_ms(100); 
    }
}