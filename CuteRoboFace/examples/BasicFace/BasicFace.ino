#include <Arduino.h>
#include <Wire.h>
#include <CuteRoboFace.h>

CuteRoboFace roboFace;

// I2C wrapper function
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
    roboFace.setBlinking(true);
    roboFace.setIdle(true);
}

void loop()
{
    roboFace.update(millis());
    delay(50);
}