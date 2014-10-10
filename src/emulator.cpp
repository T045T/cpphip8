// this is for emacs file handling -*- mode: c++; flycheck-clang-standard-library: "libc++"; flycheck-clang-language-standard: "c++11"; flycheck-clang-include-path: ("../include"); indent-tabs-mode: nil -*-

#include "cpphip8/emulator.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace cpphip8
{
Emulator::Emulator() : 
  soundTimer(0),
  delayTimer(0),
  PC(PROGRAM_START),
  SP(0),
  BEEP(false),
  highres(false),
  random_generator(),
  dist(0, 255)
{
  ClearScreen();
  setupFonts();
}

void Emulator::DisplaySprite(nibble_t x, nibble_t y, nibble_t height)
{
  byte_t xPos = registers[x];
  byte_t xOffset = xPos % 8;
  xPos /= 8;
  byte_t yPos = registers[y];
  bool killedPixel = false;

  // Special SUPER case for Extended Sprite (16x16)
  if (height == 0)
  {
    for (byte_t yOffset = 0; yOffset < 16; yOffset++)
    {
        
    }
  }
  else
  {
    
  }
  std::cout << "SPRITE! (" << (unsigned int) xPos << ", " << (unsigned int) yPos << "), height: " << (unsigned int) height << std::endl;
}

void Emulator::startEmulation()
{
  PC = PROGRAM_START;
  while(PC < 4096)
  {
    decode(readOpcode(PC));
    // Increment PC
    PC+= 2;
    // TODO(nberg): Also decrement Timers and shit
  }
}

void Emulator::loadRom(std::string path)
{
  std::ifstream file;
  file.open(path, std::ifstream::in);
  INDEX = PROGRAM_START;

  while(file.good() && INDEX < 4096)
  {
    file.read(reinterpret_cast<char*>(memory) + (INDEX), 2);
    INDEX += 2;
  }
  INDEX = 0;
}

void Emulator::ScrollDown(nibble_t x)
{

}

void Emulator::ScrollRight()
{

}

void Emulator::ScrollLeft()
{

}

void Emulator::WaitForKey(nibble_t index)
{

}

void Emulator::decode(opcode_t opcode)
{
  // Holds the opcode, split into nibbles (from left to right)
  nibble_t nibbles[4];
  for (size_t i = 0; i < 4; i++)
  {
    opcode_t mask = 0xF000 >> i*4;
    nibbles[i] = (opcode & mask) >> ((3 - i) * 4);
  }
  byte_t lowByte = opcode & 0xFF;
  address_t address = opcode & 0xFFF;

  //std::cout << std::hex << PC << ": " << opcode << std::endl;

  if ((opcode & OpCodeMasks::ScrollDown) == OpCodes::ScrollDown)
  {
    ScrollDown(nibbles[3]);
  }
  else if ((opcode & OpCodeMasks::ClearScreen) == OpCodes::ClearScreen)
  {
    ClearScreen();
  }
  else if ((opcode & OpCodeMasks::Return) == OpCodes::Return)
  {
    Return();
  }
  else if ((opcode & OpCodeMasks::ScrollRight) == OpCodes::ScrollRight)
  {
    ScrollRight();
  }
  else if ((opcode & OpCodeMasks::ScrollLeft) == OpCodes::ScrollLeft)
  {
    ScrollLeft();
  }
  else if ((opcode & OpCodeMasks::SetLowRes) == OpCodes::SetLowRes)
  {
    SetLowRes();
  }
  else if ((opcode & OpCodeMasks::SetHighRes) == OpCodes::SetHighRes)
  {
    SetHighRes();
  }
  else if ((opcode & OpCodeMasks::SysCode) == OpCodes::SysCode)
  {
    SysCode(address);
  }
  else if ((opcode & OpCodeMasks::Jump) == OpCodes::Jump)
  {
    Jump(address);
  }
  else if ((opcode & OpCodeMasks::Call) == OpCodes::Call)
  {
    Call(address);
  }
  else if ((opcode & OpCodeMasks::SkipEqualImmediate) == OpCodes::SkipEqualImmediate)
  {
    SkipEqualImmediate(nibbles[2], lowByte);
  }
  else if ((opcode & OpCodeMasks::SkipNotEqualImmediate) == OpCodes::SkipNotEqualImmediate)
  {
    SkipNotEqualImmediate(nibbles[2], lowByte);
  }
  else if ((opcode & OpCodeMasks::SkipEqualRegister) == OpCodes::SkipEqualRegister)
  {
    SkipEqualRegister(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::LoadImmediate) == OpCodes::LoadImmediate)
  {
    LoadImmediate(nibbles[1], lowByte);
  }
  else if ((opcode & OpCodeMasks::AddToRegister) == OpCodes::AddToRegister)
  {
    AddToRegister(nibbles[1], lowByte);
  }
  else if ((opcode & OpCodeMasks::LoadRegister) == OpCodes::LoadRegister)
  {
    LoadRegister(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::OR) == OpCodes::OR)
  {
    OR(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::AND) == OpCodes::AND)
  {
    AND(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::XOR) == OpCodes::XOR)
  {
    XOR(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::ADD) == OpCodes::ADD)
  {
    ADD(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::SUB) == OpCodes::SUB)
  {
    SUB(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::ShiftRight) == OpCodes::ShiftRight)
  {
    ShiftRight(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::ReverseSUB) == OpCodes::ReverseSUB)
  {
    ReverseSUB(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::ShiftLeft) == OpCodes::ShiftLeft)
  {
    ShiftLeft(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::SkipNotEqualRegister) == OpCodes::SkipNotEqualRegister)
  {
    SkipNotEqualRegister(nibbles[1], nibbles[2]);
  }
  else if ((opcode & OpCodeMasks::LoadI) == OpCodes::LoadI)
  {
    LoadI(address);
  }
  else if ((opcode & OpCodeMasks::JumpOffset) == OpCodes::JumpOffset)
  {
    JumpOffset(address);
  }
  else if ((opcode & OpCodeMasks::Random) == OpCodes::Random)
  {
    Random(nibbles[1], lowByte);
  }
  else if ((opcode & OpCodeMasks::DisplaySprite) == OpCodes::DisplaySprite)
  {
    DisplaySprite(nibbles[1], nibbles[2], nibbles[3]);
  }
  else if ((opcode & OpCodeMasks::SkipIfKey) == OpCodes::SkipIfKey)
  {
    SkipIfKey(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::SkipIfNotKey) == OpCodes::SkipIfNotKey)
  {
    SkipIfNotKey(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::LoadDelayTimer) == OpCodes::LoadDelayTimer)
  {
    LoadDelayTimer(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::WaitForKey) == OpCodes::WaitForKey)
  {
    WaitForKey(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::SetDelayTimer) == OpCodes::SetDelayTimer)
  {
    SetDelayTimer(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::SetSoundTimer) == OpCodes::SetSoundTimer)
  {
    SetSoundTimer(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::AddToI) == OpCodes::AddToI)
  {
    AddToI(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::LoadFont) == OpCodes::LoadFont)
  {
    LoadFont(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::LoadBigFont) == OpCodes::LoadBigFont)
  {
    LoadBigFont(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::StoreBCD) == OpCodes::StoreBCD)
  {
    StoreBCD(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::StoreRegisters) == OpCodes::StoreRegisters)
  {
    StoreRegisters(nibbles[1]);
  }
  else if ((opcode & OpCodeMasks::ReadRegisters) == OpCodes::ReadRegisters)
  {
    ReadRegisters(nibbles[1]);
  }
}

}

