
// Demonstrates detecting double taps, shakes, and orientation changes.

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
    writeRegister(LIS3DH_REG_CTRL2, 0xC4);    // High-pass filter (HPF) enabled for CLICK
    writeRegister(LIS3DH_REG_CTRL3, 0xC0);    // Click and IA1 interrupt signal routed to INT1 pin
    writeRegister(LIS3DH_REG_CTRL4, 0x10);    // Full Scale = +/-4 g
    writeRegister(LIS3DH_REG_CTRL5, 0x00);

    writeRegister(LIS3DH_REG_INT1DUR, 0xA0);
    writeRegister(LIS3DH_REG_INT1CFG, 0xFF);
    writeRegister(LIS3DH_REG_INT1THS, 0x0C);

    writeRegister(LIS3DH_REG_CLICKCFG, 0x20); // Double tap on Z axis
    writeRegister(LIS3DH_REG_CLICKTHS, 0x20);
    writeRegister(LIS3DH_REG_TIMELIMIT, 0x10);
    writeRegister(LIS3DH_REG_TIMELATENCY, 0x30);
    writeRegister(LIS3DH_REG_TIMEWINDOW, 0x80);
}

const char* position_description(byte int1_src)
{
    if (int1_src & 0x20) {
        return "Z UP";
    } else if (int1_src & 0x10) {
        return "Z DOWN";
    } else if (int1_src & 0x08) {
        return "Y UP";
    } else if (int1_src & 0x04) {
        return "Y DOWN";
    } else if (int1_src & 0x02) {
        return "X UP";
    } else if (int1_src & 0x01) {
        return "X DOWN";
    } else {
        return "UNKNOWN";
    }
    // Unreachable
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
        auto int1_src = readRegister(LIS3DH_REG_INT1SRC);
        auto status1 = readRegister(LIS3DH_REG_STATUS1);
        auto status2 = readRegister(LIS3DH_REG_STATUS2);
        auto ref = readRegister(LIS3DH_REG_REFERENCE);  // Dummy read to force the HP filter to set reference acceleration/tilt value

        Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;
        sensors_event_t event;
        lis.getEvent(&event);

        if (status2 & 0x80) {
            Serial.println("Overrun bit set - some data was lost");
        }

        if (int1_src & 0x3F) {
            Serial.print("Position changed to ");
            Serial.print(position_description(int1_src));
        } else {
            Serial.print("Double tap detected");
        }
        Serial.print(" int_1src 0x"); Serial.print(int1_src, HEX);
        Serial.print(" status1 0x"); Serial.print(status1, HEX);
        Serial.print(" status2 0x"); Serial.print(status2, HEX);
        Serial.print(" ref 0x"); Serial.print(ref, HEX);
        Serial.println();

        auto &acc = event.acceleration;

        /* Display the results (acceleration is measured in m/s^2) */
        Serial.print("\tX: ");
        Serial.print(acc.x);
        Serial.print(" \tY: ");
        Serial.print(acc.y);
        Serial.print(" \tZ: ");
        Serial.print(acc.z);
        Serial.println(" m/s^2 ");
        lis3dh_data_ready = false;
    }

    delay(1);
}


