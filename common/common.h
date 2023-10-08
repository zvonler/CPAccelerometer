
#ifndef common_h
#define common_h

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CircuitPlayground.h>

unsigned int readRegister(byte reg) {
    Wire1.beginTransmission(CPLAY_LIS3DH_ADDRESS);
    Wire1.write(reg);
    Wire1.endTransmission();
    Wire1.requestFrom(CPLAY_LIS3DH_ADDRESS, 1);
    return Wire1.read();
}

void writeRegister(byte reg, byte data) {
    Wire1.beginTransmission(CPLAY_LIS3DH_ADDRESS);
    Wire1.write(reg);
    Wire1.write(data);
    Wire1.endTransmission();
}

void describe_lis3dh_configuration() {
    Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;

    Serial.print("Range = ");
    Serial.print(2 << lis.getRange());
    Serial.println("G");

    Serial.print("Data rate set to: ");
    switch (lis.getDataRate()) {
        case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
        case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
        case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
        case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
        case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
        case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
        case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;

        case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
        case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
        case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
    }
}

void describe_lis3dh_status(const char* prefix) {
        auto int1_src = readRegister(LIS3DH_REG_INT1SRC);
        auto status1 = readRegister(LIS3DH_REG_STATUS1);
        auto status2 = readRegister(LIS3DH_REG_STATUS2);
        auto ref = readRegister(LIS3DH_REG_REFERENCE);  // Dummy read to force the HP filter to set reference acceleration/tilt value

        Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;
        sensors_event_t event;
        lis.getEvent(&event);

        Serial.print(prefix);
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

}

#endif
