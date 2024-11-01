add_executable(macro_rail_automater
        main.c
        )

# pull in common dependencies
target_link_libraries(macro_rail_automater pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(macro_rail_automater)
