#include "CuteRoboFace.h"

CuteRoboFace::CuteRoboFace()
{
    display = new SH1106_OLED();

    isBlinking = false;
    isIdle = false;
    isEating = false;
    isReading = false;
    mouthExpression = 0;
    blinkPhase = 0;

    eyeCenterX[0] = 40;
    eyeCenterX[1] = 85;
    eyeCenterY = 22;
    eyeRadius = 12;
    pupilX[0] = eyeCenterX[0];
    pupilX[1] = eyeCenterX[1];
    pupilY = eyeCenterY;
}

void CuteRoboFace::begin(void (*i2cWrite)(uint8_t, uint8_t *, uint16_t))
{
    display->begin(i2cWrite);
    animationStartTime = 0;
}

void CuteRoboFace::update(uint32_t currentTime)
{
    display->clear();

    if (isReading)
    {
        handleReading(currentTime);
        drawRect(70, 40, 30, 20, 1); // Book
    }
    else if (isEating)
    {
        handleEating(currentTime);
    }
    else
    {
        if (isBlinking)
            handleBlinking(currentTime);
        if (isIdle)
            handleIdle(currentTime);
    }

    drawEyes();
    drawMouth();
    display->display();
}

// ============= DRAWING HELPERS =============

void CuteRoboFace::drawPixel(uint16_t x, uint16_t y, uint8_t color)
{
    if (x >= 128 || y >= 64)
        return;
    uint16_t index = x + (y / 8) * 128;
    uint8_t bit = y % 8;
    uint8_t *buffer = display->getBuffer();
    if (color)
    {
        buffer[index] |= (1 << bit);
    }
    else
    {
        buffer[index] &= ~(1 << bit);
    }
}

void CuteRoboFace::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color)
{
    int16_t dx = abs((int16_t)x1 - (int16_t)x0);
    int16_t dy = abs((int16_t)y1 - (int16_t)y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (true)
    {
        drawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        int16_t e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void CuteRoboFace::drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint8_t color)
{
    int16_t f = 1 - (int16_t)r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * (int16_t)r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void CuteRoboFace::fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint8_t color)
{
    for (int16_t y = -r; y <= r; y++)
    {
        for (int16_t x = -r; x <= r; x++)
        {
            if (x * x + y * y <= r * r)
            {
                drawPixel(x0 + x, y0 + y, color);
            }
        }
    }
}

void CuteRoboFace::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{
    drawLine(x, y, x + w - 1, y, color);
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    drawLine(x, y, x, y + h - 1, color);
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

// ============= FACE DRAWING =============

void CuteRoboFace::drawEyes()
{
    for (uint8_t i = 0; i < 2; i++)
    {
        bool isClosed = (blinkPhase > 0 && blinkPhase < 3);
        drawEye(eyeCenterX[i], eyeCenterY, eyeRadius, pupilX[i], pupilY, isClosed);
    }
}

void CuteRoboFace::drawEye(uint16_t x, uint16_t y, uint16_t radius, uint16_t pupilX, uint16_t pupilY, bool isClosed)
{
    if (isClosed)
    {
        drawLine(x - radius, y, x + radius, y, 1);
    }
    else
    {
        drawCircle(x, y, radius, 1);
        fillCircle(pupilX, pupilY, 4, 1);
    }
}

void CuteRoboFace::drawMouth()
{
    clearMouthArea();
    switch (mouthExpression)
    {
    case 0: // NEUTRAL
        drawLine(54, 45, 74, 45, 1);
        break;
    case 1: // SMILE
        drawLine(54, 45, 57, 48, 1);
        drawLine(57, 48, 71, 48, 1);
        drawLine(71, 48, 74, 45, 1);
        break;
    case 2: // FROWN
        drawLine(54, 48, 57, 45, 1);
        drawLine(57, 45, 71, 45, 1);
        drawLine(71, 45, 74, 48, 1);
        break;
    case 3: // SURPRISED
        drawCircle(64, 45, 5, 1);
        break;
    case 4: // EATING
    {
        uint8_t phase = (animationStartTime / 500) % 4;
        if (phase == 0)
            drawMouth();
        else if (phase == 1)
            drawRect(58, 42, 12, 6, 1);
        else if (phase == 2)
            drawRect(56, 41, 16, 8, 1);
        else
        {
            drawLine(54, 45, 57, 48, 1);
            drawLine(57, 48, 71, 48, 1);
            drawLine(71, 48, 74, 45, 1);
        }
    }
    break;
    case 5: // LOVE
        drawCircle(61, 45, 3, 1);
        drawCircle(67, 45, 3, 1);
        drawLine(58, 45, 64, 50, 1);
        drawLine(64, 50, 70, 45, 1);
        break;
    case 6: // YAWN
        for (uint8_t y = 35; y < 50; y++)
        {
            for (uint8_t x = 54; x < 74; x++)
            {
                drawPixel(x, y, 1);
            }
        }
        drawPixel(54, 35, 0);
        drawPixel(73, 35, 0);
        drawPixel(54, 49, 0);
        drawPixel(73, 49, 0);
        break;
    case 7: // READING
        drawLine(58, 46, 61, 48, 1);
        drawLine(61, 48, 67, 48, 1);
        drawLine(67, 48, 70, 46, 1);
        break;
    }
}

void CuteRoboFace::clearMouthArea()
{
    for (uint8_t y = 35; y < 50; y++)
    {
        for (uint8_t x = 54; x < 74; x++)
        {
            drawPixel(x, y, 0);
        }
    }
}

// ============= EXPRESSIONS =============

void CuteRoboFace::setHappy()
{
    stopEating();
    stopReading();
    mouthExpression = 1;
}

void CuteRoboFace::setSad()
{
    stopEating();
    stopReading();
    mouthExpression = 2;
    pupilY = eyeCenterY + 3;
}

void CuteRoboFace::setAngry()
{
    stopEating();
    stopReading();
    mouthExpression = 2;
    pupilY = eyeCenterY - 2;
}

void CuteRoboFace::setTired()
{
    stopEating();
    stopReading();
    mouthExpression = 6;
}

void CuteRoboFace::setSurprised()
{
    stopEating();
    stopReading();
    mouthExpression = 3;
    eyeRadius = 14;
}

void CuteRoboFace::setLove()
{
    stopEating();
    stopReading();
    mouthExpression = 5;
}

void CuteRoboFace::eatPizza()
{
    isEating = true;
    animationStartTime = 0;
    mouthExpression = 4;
}

void CuteRoboFace::eatIceCream()
{
    isEating = true;
    animationStartTime = 0;
    mouthExpression = 4;
}

void CuteRoboFace::eatCookie()
{
    isEating = true;
    animationStartTime = 0;
    mouthExpression = 4;
}

void CuteRoboFace::readBook()
{
    isReading = true;
    animationStartTime = 0;
    mouthExpression = 7;
}

void CuteRoboFace::wink(uint8_t eye)
{
    if (eye < 2)
    {
        blinkPhase = 1;
    }
}

void CuteRoboFace::look(int8_t x, int8_t y)
{
    pupilX[0] = eyeCenterX[0] + x * 3;
    pupilX[1] = eyeCenterX[1] + x * 3;
    pupilY = eyeCenterY + y * 3;
}

void CuteRoboFace::laugh()
{
    static uint8_t laughToggle = 0;
    laughToggle = !laughToggle;
    mouthExpression = laughToggle ? 1 : 0;
}

void CuteRoboFace::confused()
{
    pupilX[0] = eyeCenterX[0] - 3;
    pupilX[1] = eyeCenterX[1] + 3;
    mouthExpression = 2;
}

void CuteRoboFace::cuteDance(uint8_t frame)
{
    pupilY = eyeCenterY + (frame % 2 == 0 ? 2 : -2);
}

void CuteRoboFace::sleepyYawn()
{
    setTired();
}

// ============= ANIMATION HANDLERS =============

void CuteRoboFace::handleBlinking(uint32_t currentTime)
{
    static uint32_t lastBlink = 0;
    if (currentTime - lastBlink > 3000)
    {
        blinkPhase = (blinkPhase + 1) % 4;
        if (blinkPhase == 0)
            lastBlink = currentTime;
    }
}

void CuteRoboFace::handleIdle(uint32_t currentTime)
{
    static uint32_t lastLook = 0;
    if (currentTime - lastLook > 100)
    {
        pupilX[0] = eyeCenterX[0] + (currentTime % 5 - 2);
        pupilX[1] = eyeCenterX[1] + (currentTime % 5 - 2);
        pupilY = eyeCenterY + (currentTime % 3 - 1);
        lastLook = currentTime;
    }
}

void CuteRoboFace::handleEating(uint32_t currentTime)
{
    animationStartTime = currentTime;
}

void CuteRoboFace::handleReading(uint32_t currentTime)
{
    static uint32_t lastLook = 0;
    if (currentTime - lastLook > 2500)
    {
        int lookDir = ((currentTime / 2500) % 2 == 0) ? -1 : 1;
        pupilX[0] = eyeCenterX[0] + lookDir * 3;
        pupilX[1] = eyeCenterX[1] + lookDir * 3;
        lastLook = currentTime;
    }
}