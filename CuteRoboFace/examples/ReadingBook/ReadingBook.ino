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
    roboFace.setIdle(false); // Disable idle for focused reading
}

void loop()
{
    // Start reading animation
    roboFace.readBook();

    // Run for 8 seconds
    unsigned long startTime = millis();
    while (millis() - startTime < 8000)
    {
        roboFace.update(millis());
        delay(50);
    }

    // Stop reading
    roboFace.stopReading();

    // Return to idle
    delay(2000);
}