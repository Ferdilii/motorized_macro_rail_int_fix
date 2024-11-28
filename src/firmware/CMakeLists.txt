add_executable(macro_rail_automater
        main.c

        acceleration.c
        backlash.c
        buttons.c
        end_point_select.c
        enter_value_widget.c
        fatal.c
        gimbal_jog.c
        gimbal_update.c
        main_led.c
        max_velocity.c
        menu.c
        motor_control.c
        motor_driver.c
        parameter_change.c
        render_common.c
        run.c
        saved_settings.c
        shot_count.c
        shutter.c
        shutter_delay.c
        )

# pull in common dependencies
target_link_libraries(macro_rail_automater
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
pico_add_extra_outputs(macro_rail_automater)
