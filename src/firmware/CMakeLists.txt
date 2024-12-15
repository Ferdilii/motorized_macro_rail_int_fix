add_executable(motorized_macro_rail
        main.c

        acceleration.c
        backlash.c
        buttons.c
        end_point_select.c
        enter_value_widget.c
        fatal.c
        gimbal_jog.c
        gimbal_update.c
        logging.c
        main_led.c
        max_velocity.c
        menu.c
        motor_control.c
        motor_driver.c
        parameter_change.c
        render_common.c
        run.c
        saved_settings.c
        settle_seconds.c
        shot_count.c
        shutter.c
        shutter_delay.c
        steps_per_mm.c
        )

# pull in common dependencies
target_link_libraries(motorized_macro_rail
    pico_multicore
    pico_stdlib
    hardware_adc
    hardware_flash
    hardware_sync

    DEBOUNCE
    FIXED_TEXT_TERMINUS8X16
    GIMBAL
    OLEDM_BITMAP
    OLEDM_FIXED_TEXT
    OLEDM_SH1107
    OLEDM_SPI
)

# create map/bin/hex file etc.
pico_add_extra_outputs(motorized_macro_rail)

# enable usb output, disable uart output
pico_enable_stdio_usb(motorized_macro_rail 1)
pico_enable_stdio_uart(motorized_macro_rail 0)
