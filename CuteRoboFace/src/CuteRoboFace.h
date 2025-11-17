#ifndef CUTE_ROBO_FACE_H
#define CUTE_ROBO_FACE_H

#include <stdint.h>
#include "SH1106_OLED.h"

class CuteRoboFace
{
private:
    SH1106_OLED *display;

    uint32_t animationStartTime;
    bool isBlinking;
    bool isIdle;
    bool isEating;
    bool isReading;
    uint8_t mouthExpression;
    uint8_t blinkPhase;

    // Eye positions
    uint16_t eyeCenterX[2];
    uint16_t eyeCenterY;
    uint16_t eyeRadius;
    uint16_t pupilX[2];
    uint16_t pupilY;

    // Drawing helpers
    void drawPixel(uint16_t x, uint16_t y, uint8_t color);
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color);
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint8_t color);
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint8_t color);
    void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color);
    void drawEye(uint16_t x, uint16_t y, uint16_t radius, uint16_t pupilX, uint16_t pupilY, bool isClosed);
    void drawMouth();
    void clearMouthArea();
    void handleBlinking(uint32_t currentTime);
    void handleIdle(uint32_t currentTime);
    void handleEating(uint32_t currentTime);
    void handleReading(uint32_t currentTime);

public:
    CuteRoboFace();
    void begin(void (*i2cWrite)(uint8_t, uint8_t *, uint16_t));
    void update(uint32_t currentTime);

    // Buffer access
    uint8_t *getBuffer() { return display->getBuffer(); }
    uint16_t getBufferSize() { return SH1106_BUFFER_SIZE; }

    // Expressions
    void setHappy();
    void setSad();
    void setAngry();
    void setTired();
    void setSurprised();
    void setLove();
    void setBlinking(bool enable) { isBlinking = enable; }
    void setIdle(bool enable) { isIdle = enable; }

    // Food animations
    void eatPizza();
    void eatIceCream();
    void eatCookie();
    void stopEating()
    {
        isEating = false;
        mouthExpression = 0;
    }

    // Reading animation
    void readBook();
    void stopReading()
    {
        isReading = false;
        mouthExpression = 0;
    }

    // Actions
    void wink(uint8_t eye = 0);
    void look(int8_t x, int8_t y);
    void laugh();
    void confused();
    void cuteDance(uint8_t frame);
    void sleepyYawn();
    void heartEyes() { setLove(); }
};

#endif