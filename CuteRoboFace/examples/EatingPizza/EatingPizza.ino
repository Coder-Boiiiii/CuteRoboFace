#include <Arduino.h>
#include <Wire.h>
#include <CuteRoboFace.h>

CuteRoboFace roboFace;

void i2cWriteWrapper(uint8_t addr, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(addr);
    Wire.write(data, len);
    Wire.endTransmission();
}

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    roboFace.begin(i2cWriteWrapper);
}

void loop()
{
    // Start pizza eating
    roboFace.eatPizza();

    // Run for 5 seconds
    unsigned long startTime = millis();
    while (millis() - startTime < 5000)
    {
        roboFace.update(millis());
        delay(50);
    }

    // Stop eating
    roboFace.stopEating();

    // Wait 2 seconds then repeat
    delay(2000);
}