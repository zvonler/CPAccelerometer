
// Demonstrates detecting orientation changes.

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

    writeRegister(LIS3DH_REG_CTRL1, 0x47);    // Enable X, Y, Z axes with ODR = 50Hz normal mode
    writeRegister(LIS3DH_REG_CTRL2, 0xC0);    // Autoreset on interrupt, HPF enabled for AOI on INT1
    writeRegister(LIS3DH_REG_CTRL3, 0x40);    // IA1 interrupt signal routed to INT1 pin
    writeRegister(LIS3DH_REG_CTRL4, 0x20);    // Full Scale = +/-8 g
    writeRegister(LIS3DH_REG_CTRL5, 0x00);

    writeRegister(LIS3DH_REG_INT1DUR, 0x0C);
    writeRegister(LIS3DH_REG_INT1CFG, 0xFF);  // Enable 6D position recognition
    writeRegister(LIS3DH_REG_INT1THS, 0x0D);

    // Interrupt will already be latched so have to clear it here
    auto int1_src = readRegister(LIS3DH_REG_INT1SRC);
}

void setup(void) {
    Serial.begin(115200);
    while (!Serial) delay(10);

    CircuitPlayground.begin();
    configure_lis3dh();
    describe_lis3dh_configuration();

    attachInterrupt(digitalPinToInterrupt(CPLAY_LIS3DH_INTERRUPT), lis3dh_ISR, RISING);

    // Interrupt will already be latched so have to clear it here
    auto int1_src = readRegister(LIS3DH_REG_INT1SRC);
}

void loop() {
    if (lis3dh_data_ready) {
        describe_lis3dh_status("Orientation changed");
        lis3dh_data_ready = false;
    }

    delay(1);
}

