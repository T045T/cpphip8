SET (CPPHIP8_SRCS
  emulator.cpp
  screen.cpp
  keyboard.cpp
)

add_library( emu
  ${CPPHIP8_SRCS}
)

add_executable( emu_test
  test.cpp
)

target_link_libraries( emu_test
  emu
)
