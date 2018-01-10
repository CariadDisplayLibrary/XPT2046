#include <XPT2046.h>

void XPT2046::initializeDevice() {
    _spi->begin();
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    pressed = false;
    pos.x = 0;
    pos.y = 0;
    _rotation = 0;
    _scale_x = 1.0;
    _scale_y = 1.0;
    _offset_x = 0;
    _offset_y = 0;
}

int XPT2046::x() {
    int x = _offset_x + ((float)(pos.x * _width / 4096) * _scale_x);
    int y = _offset_y + ((float)(pos.y * _height / 4096) * _scale_y);

    switch (_rotation) {
        case 0:
            return x;
        case 3:
            return _height - 1 - y;
        case 2:
            return _width - 1 - x;
        case 1:
            return y;
    }
    return 0;
}

int XPT2046::y() {
    int x = _offset_x + ((float)(pos.x * _width / 4096) * _scale_x);
    int y = _offset_y + ((float)(pos.y * _height / 4096) * _scale_y);

    switch (_rotation) {
        case 0:
            return y;
        case 3:
            return x;
        case 2:
            return _height - 1 - y;
        case 1:
            return _width - 1 - x;
    }
    return 0;
}


boolean XPT2046::isPressed() {
    return pressed;
}

int XPT2046::getSample(uint8_t pin) {
    _spi->setSpeed(1000000UL);
    digitalWrite(_cs, LOW);
    _spi->transfer(pin);
    uint16_t in = _spi->transfer((uint8_t)0x00) << 8;
    in |= _spi->transfer((uint8_t)0x00);
    digitalWrite(_cs, HIGH);
    return in >> 3;
}

void XPT2046::sample() {

    struct coord samples[XPT2046_SMPSIZE];
    struct coord distances[XPT2046_SMPSIZE];
    pressed = true;

    int aveX = 0;
    int aveY = 0;

    for (int i = 0; i < XPT2046_SMPSIZE; i++) {
        samples[i].x = getSample(TOUCH_CMD_X);
        samples[i].y = getSample(TOUCH_CMD_Y);
        if (samples[i].x == 0 || samples[i].x == 4095 || samples[i].y == 0 || samples[i].y == 4095) pressed = false;
        aveX += samples[i].x;
        aveY += samples[i].y;
    }

    aveX /= XPT2046_SMPSIZE;
    aveY /= XPT2046_SMPSIZE;

    for (int i = 0; i < XPT2046_SMPSIZE; i++) {
        distances[i].x = i;
        distances[i].y = ((aveX - samples[i].x) * (aveX - samples[i].x)) + ((aveY - samples[i].y) * (aveY - samples[i].y));
    }

    for (int i = 0; i < XPT2046_SMPSIZE-1; i++) {
        for (int j = 0; j < XPT2046_SMPSIZE-1; j++) {
            if (samples[j].y > samples[j+1].y) {
                int yy = samples[j+1].y;
                samples[j+1].y = samples[j].y;
                samples[j].y = yy;
                int xx = samples[j+1].x;
                samples[j+1].x = samples[j].x;
                samples[j].x = xx;
            }
        }
    }


    int tx = 0;
    int ty = 0;
    for (int i = 0; i < (XPT2046_SMPSIZE >> 1); i++) {
        tx += samples[distances[i].x].x;
        ty += samples[distances[i].x].y;
    }

    pos.x = tx / (XPT2046_SMPSIZE >> 1);
    pos.y = ty / (XPT2046_SMPSIZE >> 1);
}

void XPT2046::setRotation(int r) {
    _rotation = r % 4;
}

