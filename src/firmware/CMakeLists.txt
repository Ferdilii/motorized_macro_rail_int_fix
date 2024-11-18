add_executable(macro_rail_automater
        main.c
        main_led.c
        motor_control.c
        motor_driver.c
        fatal.c
        )

# pull in common dependencies
target_link_libraries(macro_rail_automater
    FIXED_TEXT_TERMINUS8X16
    OLEDM_BITMAP
    OLEDM_FIXED_TEXT
    OLEDM_SH1107
    OLEDM_SPI
    pico_multicore
    pico_stdlib
)

# create map/bin/hex file etc.
pico_add_extra_outputs(macro_rail_automater)
