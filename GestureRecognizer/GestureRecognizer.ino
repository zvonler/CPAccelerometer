
// Demonstrates detecting double taps, shakes, and orientation changes.

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CircuitPlayground.h>
#include "common.h"

volatile bool lis3dh_interrupted = false;

void lis3dh_ISR() {
    lis3dh_interrupted = true;
}

void configure_lis3dh(void) {
    Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;

    writeRegister(LIS3DH_REG_CTRL1, 0x77);    // Enable X, Y, Z axes with ODR = 400Hz normal mode
    writeRegister(LIS3DH_REG_CTRL2, 0xC4);    // High-pass filter (HPF) enabled for CLICK
    writeRegister(LIS3DH_REG_CTRL3, 0xC0);    // Click and IA1 interrupt signal routed to INT1 pin
    writeRegister(LIS3DH_REG_CTRL4, 0x00);    // Full Scale = +/-2 g
    writeRegister(LIS3DH_REG_CTRL5, 0x00);

    writeRegister(LIS3DH_REG_INT1DUR, 0x7F);
    writeRegister(LIS3DH_REG_INT1CFG, 0xFF);
    writeRegister(LIS3DH_REG_INT1THS, 0x32);

    writeRegister(LIS3DH_REG_CLICKCFG, 0x2A); // Double tap on any axis
    writeRegister(LIS3DH_REG_CLICKTHS, 0x50);
    writeRegister(LIS3DH_REG_TIMELIMIT, 0x06);
    writeRegister(LIS3DH_REG_TIMELATENCY, 0x32);
    writeRegister(LIS3DH_REG_TIMEWINDOW, 0x60);
}

Position position_for_int1_src(byte int1_src)
{
    if (int1_src & 0x20) return Z_UP;
    if (int1_src & 0x10) return Z_DOWN;
    if (int1_src & 0x08) return Y_UP;
    if (int1_src & 0x04) return Y_DOWN;
    if (int1_src & 0x02) return X_UP;
    if (int1_src & 0x01) return X_DOWN;
    return UNKNOWN;
}

const char* position_description(Position position)
{
    if (position == Z_UP) return "Z UP";
    if (position == Z_DOWN) return "Z DOWN";
    if (position == Y_UP) return "Y UP";
    if (position == Y_DOWN) return "Y DOWN";
    if (position == X_UP) return "X UP";
    if (position == X_DOWN) return "X DOWN";
    return "UNKNOWN";
}

void setup(void) {
    Serial.begin(115200);
    while (!Serial) delay(10);

    CircuitPlayground.begin();
    configure_lis3dh();
    describe_lis3dh_configuration();

    attachInterrupt(digitalPinToInterrupt(CPLAY_LIS3DH_INTERRUPT), lis3dh_ISR, RISING);

    // Interrupt will already be latched so have to clear it here
    //auto int1_src = readRegister(LIS3DH_REG_INT1SRC);
}

void loop() {
    uint32_t now = millis();

    constexpr static float ACCEL_DECAY = 0.9;

    static struct {
        float x = 0;
        float y = 0;
        float z = 0;

        void add_sample(float x, float y, float z)
        {
            this->x = this->x * ACCEL_DECAY + x * (1 - ACCEL_DECAY);
            this->y = this->y * ACCEL_DECAY + y * (1 - ACCEL_DECAY);
            this->z = this->z * ACCEL_DECAY + z * (1 - ACCEL_DECAY);
        }

        float total_accel() const
        {
            return sqrt(x*x + y*y + z*z);
        }
    } ewma_accel;

    static Position position = UNKNOWN;

    auto status_reg = readRegister(LIS3DH_REG_STATUS2);
    if (lis3dh_interrupted) {
        auto int1_src = readRegister(LIS3DH_REG_INT1SRC);
        auto status1 = readRegister(LIS3DH_REG_STATUS1);
        auto ref = readRegister(LIS3DH_REG_REFERENCE);  // Dummy read to force the HP filter to set reference acceleration/tilt value
        auto click_src = readRegister(LIS3DH_REG_CLICKSRC);

        Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;
        sensors_event_t event;
        lis.getEvent(&event);
        auto &acc = event.acceleration;
        ewma_accel.add_sample(acc.x, acc.y, acc.z);

        if (int1_src & 0x3F) {
            auto new_position = position_for_int1_src(int1_src);
            if (position != new_position) {
                position = new_position;
                Serial.print("Position changed to ");
                Serial.println(position_description(position));
            }
        }

        if (click_src & 0x60) {
            Serial.println("Double tap detected");
        }

        // Serial.print("state");
        // Serial.print(" int_1src 0x"); Serial.print(int1_src, HEX);
        // Serial.print(" status1 0x"); Serial.print(status1, HEX);
        // Serial.print(" status_reg 0x"); Serial.print(status_reg, HEX);
        // Serial.print(" ref 0x"); Serial.print(ref, HEX);
        // Serial.print(" click_src 0x"); Serial.print(click_src, HEX);
        // Serial.println();

        lis3dh_interrupted = false;
    } else if (status_reg & 0x08) { // This bit indicates new data is available on all three axes
        Adafruit_CPlay_LIS3DH& lis = CircuitPlayground.lis;
        sensors_event_t event;
        lis.getEvent(&event);
        auto &acc = event.acceleration;
        ewma_accel.add_sample(acc.x, acc.y, acc.z);

        static uint32_t last_shake_tm = 0;
        if (ewma_accel.total_accel() > 20 && now - last_shake_tm >= 500) {
            Serial.println("Shake detected");
            last_shake_tm = now;
        }
    }

    static uint32_t last_print_tm = 0;
    if (now - last_print_tm >= 500) {
        Serial.print("\tX: ");
        Serial.print(ewma_accel.x);
        Serial.print(" \tY: ");
        Serial.print(ewma_accel.y);
        Serial.print(" \tZ: ");
        Serial.print(ewma_accel.z);
        Serial.println(" m/s^2 ");
        last_print_tm = now;
    }

    delay(1);
}


