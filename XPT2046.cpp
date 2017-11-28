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
            return _height - y;
        case 2:
            return _width - x;
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
            return _height - y;
        case 1:
            return _width - x;
    }
    return 0;
}


boolean XPT2046::isPressed() {
    return pressed;
}

int XPT2046::getSample(uint8_t pin) {
    int samples[XPT2046_SMPSIZE];
    _spi->setSpeed(1000000UL);
    pressed = true;
    uint32_t smpTot = 0;
    for (int i = 0; i < XPT2046_SMPSIZE; i++) {
        digitalWrite(_cs, LOW);
        _spi->transfer(pin);
        uint16_t in = _spi->transfer((uint8_t)0x00) << 8;
        in |= _spi->transfer((uint8_t)0x00);
        digitalWrite(_cs, HIGH);
        samples[i] = in >> 3;
        if (samples[i] == 0 || samples[i] == 4095) pressed = false;
        smpTot += (in >> 3);
    }

    smpTot = smpTot / XPT2046_SMPSIZE;
    return smpTot;
}

void XPT2046::sample() {

    pos.x = getSample(TOUCH_CMD_X);
    pos.y = getSample(TOUCH_CMD_Y);
}

void XPT2046::setRotation(int r) {
    _rotation = r % 4;
}

