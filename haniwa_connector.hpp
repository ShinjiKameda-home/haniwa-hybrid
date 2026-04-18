#ifndef HANIWA_CONNECTOR_HPP
#define HANIWA_CONNECTOR_HPP

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

bool haniwa_connector_init();
void haniwa_send_hello();
void haniwa_send_data(uint16_t moisture);
bool haniwa_recv_result(LEDStatus* out_status);
void haniwa_poll_result();

#endif