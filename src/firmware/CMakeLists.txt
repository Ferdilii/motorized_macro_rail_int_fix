add_executable(macro_rail_automater
        main.c

        end_point_select.c
        fatal.c
        gimbal_update.c
        main_led.c
        motor_control.c
        motor_driver.c
        )

# pull in common dependencies
target_link_libraries(macro_rail_automater
    pico_multicore
    pico_stdlib
    hardware_adc

    FIXED_TEXT_TERMINUS8X16
    GIMBAL
    OLEDM_BITMAP
    OLEDM_FIXED_TEXT
    OLEDM_SH1107
    OLEDM_SPI
)

# create map/bin/hex file etc.
pico_add_extra_outputs(macro_rail_automater)
