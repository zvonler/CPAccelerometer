
// Demonstrates using the data ready interrupt instead of polling.

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CircuitPlayground.h>
#include "common.h"

volatile bool lis3dh_data_ready = false;

void lis3dh_ISR() {
    lis3dh_data_ready = true;
}

void configure_lis3dh(void) {
    Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;

    writeRegister(LIS3DH_REG_CTRL1, 0x1F);    // Enable X, Y, Z axes with ODR = 1Hz 8-bit low-power mode
    writeRegister(LIS3DH_REG_CTRL2, 0x09);    // High-pass filter (HPF) enabled
    writeRegister(LIS3DH_REG_CTRL3, 0x10);    // Enable I1_ZYXDA
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
    configure_lis3dh();
    describe_lis3dh_configuration();

    attachInterrupt(digitalPinToInterrupt(CPLAY_LIS3DH_INTERRUPT), lis3dh_ISR, RISING);

    // This initial read resets the interrupt state
    describe_lis3dh_status("Initial");
}

void loop() {
    if (lis3dh_data_ready) {
	describe_lis3dh_status("Data ready");
        lis3dh_data_ready = false;
    }

    delay(10);
}

