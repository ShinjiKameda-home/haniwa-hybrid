#include "pico/stdlib.h"

// ハニワさんの視神経（GPIO番号）を定義
const uint LED_RED   = 13;
const uint LED_GREEN = 14;
const uint LED_BLUE  = 15;

void blink_led(uint gpio, int duration_sec) {
    // 指定された秒数（duration_sec）の間、0.5秒おきに点滅
    int blinks = duration_sec / 1; // 1サイクル（ON 0.5s + OFF 0.5s）
    for (int i = 0; i < blinks * 5; i++) { // 合計5秒間になるように調整
        gpio_put(gpio, 1); // ON
        sleep_ms(500);
        gpio_put(gpio, 0); // OFF
        sleep_ms(500);
    }
}

int main() {
    // SDKの初期化
    stdio_init_all();

    // 各ピンを出力モードとして初期化
    const uint leds[] = {LED_RED, LED_GREEN, LED_BLUE};
    for (uint led : leds) {
        gpio_init(led);
        gpio_set_dir(led, GPIO_OUT);
    }

    while (true) {
        // 赤を5秒間点滅
        blink_led(LED_RED, 5);
        
        // 緑を5秒間点滅
        blink_led(LED_GREEN, 5);
        
        // 青を5秒間点滅
        blink_led(LED_BLUE, 5);
    }
}