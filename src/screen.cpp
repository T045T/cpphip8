// this is for emacs file handling -*- mode: c++; flycheck-clang-standard-library: "libc++"; flycheck-clang-language-standard: "c++11"; flycheck-clang-include-path: ("../include"); indent-tabs-mode: nil -*-

#include "cpphip8/screen.hpp"
#include <iostream>

namespace cpphip8
{
Screen::Screen() :
  currentXRes(64),
  currentYRes(32)
{
  // Trollolol
}

void Screen::refresh(uint8_t (&framebuffer)[128/8][64])
{
  for (size_t vblank = 0; vblank < 5; vblank++)
  {
    std::cout << "\n";
  }
  std::cout << "----------------------------------------------------------------------------------------------------------------------------------\n";
  for (size_t y = 0; y < 32; y++)
  {
    std::cout << "|";
    for (size_t x = 0; x < 64; x+= 8)
    {
      for (int i = 7; i >= 0; i--)
      {
        std::cout << (std::bitset<8>(framebuffer[x / 8][y])[i] ? "##" : "  ");
      }
    }
    std::cout << "|\n";
  }
  std::cout << "----------------------------------------------------------------------------------------------------------------------------------\n";
  std::cout.flush();
}
}
