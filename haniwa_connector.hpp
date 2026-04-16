#ifndef HANIWA_CONNECTOR_HPP
#define HANIWA_CONNECTOR_HPP

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

bool haniwa_connector_init();
void haniwa_connector_send_hello();

#endif