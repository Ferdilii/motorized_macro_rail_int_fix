## Changling OLED displays

There is a not-fully-tested effort in place to support different OLEDs.

In `CMakeLists.txt` there is

    OLEDM_SH1107
    OLEDM_SPI

These can be swapped out with included alternatives that are untested
for this project but have seen plenty of use in other ones:

    OLEDM_TWI  # for I2C displays
    OLEDM_SH1106
    OLEDM_SSD1306

## Changing Fonts

The font

    FIXED_TEXT_TERMINUS8X16

can be changed to

    FIXED_TEXT_TERMINUS8X8

If using an 128x64 OLED instead of a 128x128 one.  You'll need to change the
following in `shared_state.h` at a minimum and will likely need further
tweaks to get it all working:

from

    #define DISPLAY_WIDTH 128
    #define DISPLAY_HEIGHT 128
    #define DISPLAY_ROWS 16
    #define TEXT_HEIGHT 16
    #define TEXT_FONT terminus8x16

to

    #define DISPLAY_WIDTH 128
    #define DISPLAY_HEIGHT 64
    #define DISPLAY_ROWS 8
    #define TEXT_HEIGHT 8
    #define TEXT_FONT terminus8x8

