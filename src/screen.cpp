// this is for emacs file handling -*- mode: c++; flycheck-clang-standard-library: "libc++"; flycheck-clang-language-standard: "c++11"; flycheck-clang-include-path: ("../include"); indent-tabs-mode: nil -*-

#include "cpphip8/screen.hpp"
#include <iostream>

namespace cpphip8
{
Screen::Screen()
{
  // Trollolol
}

void Screen::refresh(uint8_t** framebuffer)
{
  for (size_t vblank = 0; vblank < 32; vblank++)
  {
    std::cout << "\n";
  }
  for (size_t y = 0; y < 32; y++)
  {
    for (size_t x = 0; x < 64; x+= 8)
    {
      std::cout << std::bitset<8>(framebuffer[x / 8][y]);
    }
    std::cout << "\n";
  }
  std::cout.flush();
}
}
