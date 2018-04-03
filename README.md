XPT2046 Touchscreen Controller
==============================

This is a driver for the XPT2046 SPI touchscreen interface controller using
the DSPI SPI library.

Usage:
------

Construction:

    DSPI0 spi;
    XPT2046 ts(spi, chipselect, width, height);

All other functions are the standard Touch class functions. There are no device-specific functions.


