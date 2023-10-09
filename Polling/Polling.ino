
// Demonstrates reading data and checking the status register.

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CircuitPlayground.h>
#include "common.h"

void configure_lis3dh(void) {
    Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;

    writeRegister(LIS3DH_REG_CTRL1, 0x2F);    // Enable X, Y, Z axes with ODR = 10Hz 8-bit low-power mode
    writeRegister(LIS3DH_REG_CTRL2, 0x09);    // High-pass filter (HPF) enabled
    writeRegister(LIS3DH_REG_CTRL3, 0x00);
    writeRegister(LIS3DH_REG_CTRL4, 0x20);    // Full Scale = +/-8 g
    writeRegister(LIS3DH_REG_CTRL5, 0x00);
    writeRegister(LIS3DH_REG_CTRL6, 0x00);

    writeRegister(LIS3DH_REG_REFERENCE, 0x00);

    writeRegister(LIS3DH_REG_INT1THS, 0x00);
    writeRegister(LIS3DH_REG_INT1DUR, 0x00);
    writeRegister(LIS3DH_REG_INT1CFG, 0x00);

    writeRegister(LIS3DH_REG_CTRL5, 0x00);
}

void setup(void) {
    Serial.begin(115200);
    while (!Serial) delay(10);

    CircuitPlayground.begin();
    Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;
    configure_lis3dh();

    describe_lis3dh_configuration();
}

void loop() {
    auto status_reg = readRegister(LIS3DH_REG_STATUS2);

    if (status_reg & 0x08) { // This bit indicates new data is available on all three axes

        if (status_reg & 0x80) {
            // If the read function (i.e. getEvent) is not called frequently
            // enough, this bit will be set. With a 10Hz configuration and
            // a 100ms delay in loop(), this will happen occasionally.
            Serial.println("Some data was lost");
        }

        Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;
        sensors_event_t event;
        lis.getEvent(&event);

        Serial.print("Data ready");

        /* Display the results (acceleration is measured in m/s^2) */
        auto &acc = event.acceleration;
        Serial.print("\tX: ");
        Serial.print(acc.x);
        Serial.print(" \tY: ");
        Serial.print(acc.y);
        Serial.print(" \tZ: ");
        Serial.print(acc.z);
        Serial.println(" m/s^2 ");
    } else {
        Serial.println("Not ready");
    }

    delay(100);
}


