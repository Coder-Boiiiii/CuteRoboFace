#ifndef SH1106_OLED_H
#define SH1106_OLED_H

#include <stdint.h>

#define SH1106_I2C_ADDRESS 0x3C
#define SH1106_WIDTH 128
#define SH1106_HEIGHT 64
#define SH1106_BUFFER_SIZE (SH1106_WIDTH * SH1106_HEIGHT / 8)

class SH1106_OLED
{
private:
    uint8_t buffer[SH1106_BUFFER_SIZE];
    void (*i2cWriteFunc)(uint8_t addr, uint8_t *data, uint16_t len);

public:
    SH1106_OLED();
    void begin(void (*i2cWrite)(uint8_t, uint8_t *, uint16_t));
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t *data, uint16_t length);
    void display();
    void clear();
    uint8_t *getBuffer();
};

#endif