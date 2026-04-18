#ifndef HANIWA_MAIN_HPP
#define HANIWA_MAIN_HPP

enum LEDStatus {
    STATUS_SKIP = 0,    // Green: SKip watering
    STATUS_GO = 1,      // Red: GO watering
    STATUS_TOO_MUCH = 2 // Blue: TOO_MUCH water
};

#endif