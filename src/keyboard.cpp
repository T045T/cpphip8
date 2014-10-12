// this is for emacs file handling -*- mode: c++; flycheck-clang-standard-library: "libc++"; flycheck-clang-language-standard: "c++11"; flycheck-clang-include-path: ("../include"); indent-tabs-mode: nil -*-

#include "cpphip8/keyboard.hpp"

namespace cpphip8
{
Keyboard::Keyboard()
{
  for(size_t i = 0; i < 16; i++)
  {
    keys[i] = false;
  }
  mythread = std::thread(&Keyboard::inputLoop, this);
}

Keyboard::~Keyboard()
{
  {
    std::lock_guard<std::mutex> lock(m);
    alive = false;
  }
  mythread.join();
}

bool Keyboard::pollKey(uint8_t key)
{
  std::lock_guard<std::mutex> lock(m);
  return keys[key];
}

void Keyboard::inputLoop()
{
  while(alive)
  {
    std::lock_guard<std::mutex> lock(m);
    for(size_t i = 0; i < 16; i++)
    {
      
    }
  }
}

}
