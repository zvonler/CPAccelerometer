
// Demonstrates detecting taps or double taps.

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

    writeRegister(LIS3DH_REG_CTRL1, 0x77);    // Enable X, Y, Z axes with ODR = 400Hz low-power mode
    writeRegister(LIS3DH_REG_CTRL2, 0x04);    // High-pass filter (HPF) enabled for CLICK
    writeRegister(LIS3DH_REG_CTRL3, 0x80);    // Click interrupt signal routed to INT1 pin
    writeRegister(LIS3DH_REG_CTRL4, 0x20);    // Full Scale = +/-8 g
    writeRegister(LIS3DH_REG_CTRL5, 0x00);

    writeRegister(LIS3DH_REG_INT1DUR, 0x00);  // Duration 0 since latching interrupts
    writeRegister(LIS3DH_REG_INT1CFG, 0x00);

    writeRegister(LIS3DH_REG_CLICKCFG, 0x20); // Double tap on Z axis
    writeRegister(LIS3DH_REG_CLICKTHS, 0x27);
    writeRegister(LIS3DH_REG_TIMELIMIT, 0x06);
    writeRegister(LIS3DH_REG_TIMELATENCY, 0x20);
    writeRegister(LIS3DH_REG_TIMEWINDOW, 0x70);
}

void setup(void) {
    Serial.begin(115200);
    while (!Serial) delay(10);

    CircuitPlayground.begin();
    configure_lis3dh();
    describe_lis3dh_configuration();

    attachInterrupt(digitalPinToInterrupt(CPLAY_LIS3DH_INTERRUPT), lis3dh_ISR, RISING);
}

void loop() {
    if (lis3dh_data_ready) {
        describe_lis3dh_status("Double tap detected");
        lis3dh_data_ready = false;
    }

    delay(1);
}

