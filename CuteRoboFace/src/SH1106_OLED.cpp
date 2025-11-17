#include "SH1106_OLED.h"
#include <string.h>

SH1106_OLED::SH1106_OLED()
{
    memset(buffer, 0, SH1106_BUFFER_SIZE);
    i2cWriteFunc = nullptr;
}

void SH1106_OLED::begin(void (*i2cWrite)(uint8_t, uint8_t *, uint16_t))
{
    i2cWriteFunc = i2cWrite;

    uint8_t initCmds[] = {
        0xAE, 0xDC, 0x00, 0x81, 0x2F, 0x20, 0x10, 0xA0, 0xC0,
        0xA6, 0xA8, 0x3F, 0xD3, 0x60, 0xD5, 0x51, 0xD9, 0x22,
        0xDB, 0x35, 0xA4, 0xAF};
    for (uint8_t i = 0; i < sizeof(initCmds); i++)
    {
        sendCommand(initCmds[i]);
    }
}

void SH1106_OLED::sendCommand(uint8_t cmd)
{
    if (!i2cWriteFunc)
        return;
    uint8_t data[2] = {0x80, cmd};
    i2cWriteFunc(SH1106_I2C_ADDRESS, data, 2);
}

void SH1106_OLED::sendData(uint8_t *data, uint16_t length)
{
    if (!i2cWriteFunc)
        return;
    uint8_t *packet = new uint8_t[length + 1];
    packet[0] = 0x40;
    memcpy(&packet[1], data, length);
    i2cWriteFunc(SH1106_I2C_ADDRESS, packet, length + 1);
    delete[] packet;
}

void SH1106_OLED::display()
{
    if (!i2cWriteFunc)
        return;
    for (uint8_t page = 0; page < 8; page++)
    {
        sendCommand(0xB0 + page);
        sendCommand(0x02);
        sendCommand(0x10);
        sendData(&buffer[page * 128], 128);
    }
}

void SH1106_OLED::clear()
{
    memset(buffer, 0, SH1106_BUFFER_SIZE);
}

uint8_t *SH1106_OLED::getBuffer()
{
    return buffer;
}