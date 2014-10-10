// this is for emacs file handling -*- mode: c++; flycheck-clang-standard-library: "libc++"; flycheck-clang-language-standard: "c++11"; flycheck-clang-include-path: ("../include"); indent-tabs-mode: nil -*-

#include "cpphip8/emulator.hpp"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    return 1;
  }
  cpphip8::Emulator emu;
  std::string romPath = argv[1];
  emu.loadRom(romPath);
  emu.startEmulation();
}
