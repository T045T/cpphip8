CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(CPPHIP8)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES( 
  "include"
  ${SDL2_INCLUDE_DIRS}
  )
SET( CMAKE_CXX_FLAGS 
  "-std=c++11 ${CMAKE_CXX_FLAGS} -Wall -g"
)

SET (CPPHIP8_SRCS
  src/emulator.cpp
)

add_library( emu
  ${CPPHIP8_SRCS}
)

add_executable( emu_test
  src/test.cpp
)

target_link_libraries( emu
  ${SDL2_LIBRARIES}
)

target_link_libraries( emu_test
  emu
)
