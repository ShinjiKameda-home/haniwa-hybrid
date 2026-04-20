# HANIWA Guardian

The HANIWA Guardian is a hybrid IoT system bridging ancient Japanese terracotta art with modern Edge AI. It functions as a smart irrigation advisor and an active scarecrow, powered by a Raspberry Pi Pico 2 W and a distributed Python backend.

## 📖 Full Documentation & Story
For the complete build guide, system architecture, and the philosophy behind this project, please visit the official article on Hackster.io:

👉 **[The HANIWA Guardian: Timeless yet Cutting-edge Terracotta Irrigation and Smart Scarecrow Hybrid System](https://www.hackster.io/shinji_kameda/the-haniwa-timeless-yet-cutting-edge-irrigatable-scarecrow-57b38a)**

## 🛠️ Quick Tech Specs
- **MCU**: Raspberry Pi Pico 2 W
- **Language**: C++ (Pico SDK) & Python 3.10+
- **Key Techs**: TCP/IP (WiFi), Shared Memory, OpenCV, OpenWeatherMap API
- **Hardware**: Capacitive Soil Moisture Sensor, RGB LEDs, Haniwa Replica

## 📂 Repository Structure
- `/pico_firmware`: C++ source code for the microcontroller.
- `/python_services`: Backend intelligence (Weather API, AI Vision, Gateway).

