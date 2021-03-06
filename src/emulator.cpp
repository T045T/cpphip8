// this is for emacs file handling -*- mode: c++; flycheck-clang-standard-library: "libc++"; flycheck-clang-language-standard: "c++11"; flycheck-clang-include-path: ("../include" "/usr/local/include/SDL2"); indent-tabs-mode: nil -*-

#include "cpphip8/emulator.hpp"
#include <chrono>
#include <cstring>
#include <thread>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <bitset>

namespace cpphip8
{
Emulator::Emulator() : 
  soundTimer(0),
  delayTimer(0),
  SP(0),
  BEEP(false),
  highres(false),
  random_generator(),
  dist(0, 255),
  screenUpdate(false),
  soundRequest(false),
  keys()
{
  init();
  memset(memory, 0, 4096);
}

void Emulator::init()
{
  PC = PROGRAM_START;
  // First push will increment SP (to 0), then save PC
  SP = 0xFF;
  INDEX = 0;
  new_keypress = 0xFF;

  soundTimer = 0;
  delayTimer = 0;
  BEEP = false;

  highres = false;

  screenUpdate = false;
  soundRequest = false;

  memset(keys, false, 16);
  memset(stack, 0, 16);
  memset(registers, 0, 16);
  ClearScreen();
}

void Emulator::startEmulation()
{
  init();
  alive = true;
  paused = false;
  // Initialize to invalid value, so waitForKey doesn't trigger
  new_keypress = 0xFF;
  t = std::thread(&Emulator::mainLoop, this);
}

void Emulator::stopEmulation()
{
  {
    std::lock_guard<std::mutex> lock(m);
    new_keypress = 0;
    alive = false;
  }
  // Wake up any waiting waitForKey instruction
  key_cv.notify_one();
  t.join();
  paused = true;
}

void Emulator::pauseEmulation()
{
  std::lock_guard<std::mutex> lock(m);
  paused = true;
}

void Emulator::resumeEmulation()
{
  {
    std::lock_guard<std::mutex> lock(m);
    paused = false;
  }
  pause_cv.notify_one();
}

void Emulator::keyDown(uint8_t key)
{
  // std::cout << "[Backend] Key Down: " << (int) key << std::endl;
  std::lock_guard<std::mutex> lock(m);
  keys[key] = true;
  new_keypress = key;
  key_cv.notify_one();
}

void Emulator::keyUp(uint8_t key)
{
  // std::cout << "[Backend] Key Up: " << (int) key << std::endl;
  std::lock_guard<std::mutex> lock(m);
  keys[key] = false;
}

void Emulator::tick()
{
  if (delayTimer > 0)
  {
    delayTimer--;
  }
  if (soundTimer > 0)
  {
    soundTimer--;
  }
}

void Emulator::mainLoop()
{
  unsigned int counter = 0;
  while (alive)
  {
    {
      std::unique_lock<std::mutex> lock(m);
      while(paused)
      {
        pause_cv.wait(lock);
      }
    }

    // std::cout << "@ 0x" << std::hex << (int) PC << ": " << (int) (memory[PC] >> 4) << (int) (memory[PC] & 0xF) << (int) (memory[PC+1] >> 4) << (int) (memory[PC+1] & 0xF);
    
    //uint16_t opcode = 
    decode(readOpcode(PC));

    // std::cout << " opcode: " << std::hex << (int) opcode
    //           << " registers after: ";
    // for(unsigned int i = 0; i < 16; i++)
    // {
    //   std::cout << "V" << i << "=" << (int) registers[i] << " ";
    // }
    // std::cout << std::endl;
    counter++;
    // Increment PC
    PC+= 2;
    if (PC >= 4096)
    {
      alive = false;
      break;
    }
    // TODO(nberg): Also increment Timers and shit
    if (counter % 15 == 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(15));
      if (delayTimer > 0)
      {
        delayTimer--;
      }
      if (soundTimer > 0)
      {
        soundTimer--;
      }
    }
  }
}

void Emulator::loadRom(std::string path)
{
  // First, clear memory
  memset(memory, 0, 4096);

  std::ifstream file;
  file.open(path, std::ifstream::in);
  INDEX = PROGRAM_START;

  while(file.good() && INDEX < 4096)
  {
    file.read(reinterpret_cast<char*>(memory) + (INDEX), 2);
    INDEX += 2;
  }
  INDEX = 0;

  // Finally, copy font data
  setupFonts();
}


void Emulator::DisplaySprite(nibble_t x, nibble_t y, nibble_t height)
{
  byte_t xPos = registers[x];
  byte_t xOffset = xPos % 8;
  xPos /= 8;

  // Wrapping
  xPos = xPos % (getXRes() / 8);
  byte_t yPos = registers[y] % getYRes();
  
  bool collision = false;

  // std::cout << std::dec << "Drawing: (" << (int) xPos*8 + xOffset << ", " << (int) yPos << ") " << (int) height << std::endl;
  
  // Special SUPER case for Extended Sprite (16x16)
  if (height == 0)
  {
    for (byte_t yOffset = 0; yOffset < 16; yOffset++)
    {
        
    }
  }
  else
  {
    byte_t left_byte;
    byte_t right_byte;
    byte_t right_xPos = (xPos + 1);
    for (byte_t yOffset = 0; yOffset < height && yPos + yOffset < getYRes(); yOffset++)
    {
      left_byte = memory[INDEX + yOffset] >> xOffset;
      // if xOffset == 0, right_byte ends up empty and doesn't affect anything in the XOR
      right_byte = memory[INDEX + yOffset] << (8 - xOffset);
      byte_t tmp_left = frameBuffer[xPos][(yPos + yOffset)];
      frameBuffer[xPos][(yPos + yOffset)] ^= left_byte;
      if ( tmp_left & left_byte)
      {
        collision = true;
      }
      if (right_xPos < getXRes() / 8)
      {
        byte_t tmp_right = frameBuffer[right_xPos][(yPos + yOffset)];
        frameBuffer[right_xPos][(yPos + yOffset)] ^= right_byte;
        if (tmp_right & right_byte)
        {
          collision = true;
        }
      }
    }
  }
  
  if(collision)
  {
    // std::cout << "Collision" << std::endl;
    registers[RegisterNames::VF] = 1;
    }
  else
  {
    // std::cout << "NO Collision" << std::endl;
    registers[RegisterNames::VF] = 0;
  }

  //  std::cout << "SPRITE! (" << (unsigned int) xPos << ", " << (unsigned int) yPos << "), height: " << (unsigned int) height << std::endl;
  std::lock_guard<std::mutex> lock(m);
  screenUpdate = true;
}

bool Emulator::getPixel(unsigned int x, unsigned int y)
{
  // Pixels outside of the screen are always off
  if (x > getXRes() || y > getYRes())
  {
    return false;
  }

  std::bitset<8> bs(frameBuffer[x/8][y]);
  return bs[7-(x % 8)];
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
  // std::cout << "Waiting for key" << std::endl;
  std::unique_lock<std::mutex> lock(m);
  while(new_keypress > 0xF)
  {
    key_cv.wait(lock);
  }
  registers[index] = new_keypress;
  // std::cout << "key pressed: " << (int) new_keypress << std::endl;
  // set new_keypress to non-key value
  new_keypress = 0xFF;
}

OpCodes::OpCode Emulator::decode(opcode_t opcode)
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

//  std::cout << std::hex << PC << ": " << opcode << std::endl;

  if ((opcode & OpCodeMasks::ScrollDown) == OpCodes::ScrollDown)
  {
    ScrollDown(nibbles[3]);
    return OpCodes::ScrollDown;
  }
  else if ((opcode & OpCodeMasks::ClearScreen) == OpCodes::ClearScreen)
  {
    ClearScreen();
    return OpCodes::ClearScreen;
  }
  else if ((opcode & OpCodeMasks::Return) == OpCodes::Return)
  {
    Return();
    return OpCodes::Return;
  }
  else if ((opcode & OpCodeMasks::ScrollRight) == OpCodes::ScrollRight)
  {
    ScrollRight();
    return OpCodes::ScrollRight;
  }
  else if ((opcode & OpCodeMasks::ScrollLeft) == OpCodes::ScrollLeft)
  {
    ScrollLeft();
    return OpCodes::ScrollLeft;
  }
  else if ((opcode & OpCodeMasks::SetLowRes) == OpCodes::SetLowRes)
  {
    SetLowRes();
    return OpCodes::SetLowRes;
  }
  else if ((opcode & OpCodeMasks::SetHighRes) == OpCodes::SetHighRes)
  {
    SetHighRes();
    return OpCodes::SetHighRes;
  }
  else if ((opcode & OpCodeMasks::SysCode) == OpCodes::SysCode)
  {
    SysCode(address);
    return OpCodes::SysCode;
  }
  else if ((opcode & OpCodeMasks::Jump) == OpCodes::Jump)
  {
    Jump(address);
    return OpCodes::Jump;
  }
  else if ((opcode & OpCodeMasks::Call) == OpCodes::Call)
  {
    Call(address);
    return OpCodes::Call;
  }
  else if ((opcode & OpCodeMasks::SkipEqualImmediate) == OpCodes::SkipEqualImmediate)
  {
    SkipEqualImmediate(nibbles[1], lowByte);
    return OpCodes::SkipEqualImmediate;
  }
  else if ((opcode & OpCodeMasks::SkipNotEqualImmediate) == OpCodes::SkipNotEqualImmediate)
  {
    SkipNotEqualImmediate(nibbles[1], lowByte);
    return OpCodes::SkipNotEqualImmediate;
  }
  else if ((opcode & OpCodeMasks::SkipEqualRegister) == OpCodes::SkipEqualRegister)
  {
    SkipEqualRegister(nibbles[1], nibbles[2]);
    return OpCodes::SkipEqualRegister;
  }
  else if ((opcode & OpCodeMasks::LoadImmediate) == OpCodes::LoadImmediate)
  {
    LoadImmediate(nibbles[1], lowByte);
    return OpCodes::LoadImmediate;
  }
  else if ((opcode & OpCodeMasks::AddToRegister) == OpCodes::AddToRegister)
  {
    AddToRegister(nibbles[1], lowByte);
    return OpCodes::AddToRegister;
  }
  else if ((opcode & OpCodeMasks::LoadRegister) == OpCodes::LoadRegister)
  {
    LoadRegister(nibbles[1], nibbles[2]);
    return OpCodes::LoadRegister;
  }
  else if ((opcode & OpCodeMasks::OR) == OpCodes::OR)
  {
    OR(nibbles[1], nibbles[2]);
    return OpCodes::OR;
  }
  else if ((opcode & OpCodeMasks::AND) == OpCodes::AND)
  {
    AND(nibbles[1], nibbles[2]);
    return OpCodes::AND;
  }
  else if ((opcode & OpCodeMasks::XOR) == OpCodes::XOR)
  {
    XOR(nibbles[1], nibbles[2]);
    return OpCodes::XOR;
  }
  else if ((opcode & OpCodeMasks::ADD) == OpCodes::ADD)
  {
    ADD(nibbles[1], nibbles[2]);
    return OpCodes::ADD;
  }
  else if ((opcode & OpCodeMasks::SUB) == OpCodes::SUB)
  {
    SUB(nibbles[1], nibbles[2]);
    return OpCodes::SUB;
  }
  else if ((opcode & OpCodeMasks::ShiftRight) == OpCodes::ShiftRight)
  {
    ShiftRight(nibbles[1]);
    return OpCodes::ShiftRight;
  }
  else if ((opcode & OpCodeMasks::ReverseSUB) == OpCodes::ReverseSUB)
  {
    ReverseSUB(nibbles[1], nibbles[2]);
    return OpCodes::ReverseSUB;
  }
  else if ((opcode & OpCodeMasks::ShiftLeft) == OpCodes::ShiftLeft)
  {
    ShiftLeft(nibbles[1]);
    return OpCodes::ShiftLeft;
  }
  else if ((opcode & OpCodeMasks::SkipNotEqualRegister) == OpCodes::SkipNotEqualRegister)
  {
    SkipNotEqualRegister(nibbles[1], nibbles[2]);
    return OpCodes::SkipNotEqualRegister;
  }
  else if ((opcode & OpCodeMasks::LoadI) == OpCodes::LoadI)
  {
    LoadI(address);
    return OpCodes::LoadI;
  }
  else if ((opcode & OpCodeMasks::JumpOffset) == OpCodes::JumpOffset)
  {
    JumpOffset(address);
    return OpCodes::JumpOffset;
  }
  else if ((opcode & OpCodeMasks::Random) == OpCodes::Random)
  {
    Random(nibbles[1], lowByte);
    return OpCodes::Random;
  }
  else if ((opcode & OpCodeMasks::DisplaySprite) == OpCodes::DisplaySprite)
  {
    DisplaySprite(nibbles[1], nibbles[2], nibbles[3]);
    return OpCodes::DisplaySprite;
  }
  else if ((opcode & OpCodeMasks::SkipIfKey) == OpCodes::SkipIfKey)
  {
//    std::cout << "hi" << std::endl;
    SkipIfKey(nibbles[1]);
    return OpCodes::SkipIfKey;
  }
  else if ((opcode & OpCodeMasks::SkipIfNotKey) == OpCodes::SkipIfNotKey)
  {
    //    std::cout << "hey! hello!" << std::endl;
    SkipIfNotKey(nibbles[1]);
    return OpCodes::SkipIfNotKey;
  }
  else if ((opcode & OpCodeMasks::LoadDelayTimer) == OpCodes::LoadDelayTimer)
  {
    LoadDelayTimer(nibbles[1]);
    return OpCodes::LoadDelayTimer;
  }
  else if ((opcode & OpCodeMasks::WaitForKey) == OpCodes::WaitForKey)
  {
    WaitForKey(nibbles[1]);
    return OpCodes::WaitForKey;
  }
  else if ((opcode & OpCodeMasks::SetDelayTimer) == OpCodes::SetDelayTimer)
  {
    SetDelayTimer(nibbles[1]);
    return OpCodes::SetDelayTimer;
  }
  else if ((opcode & OpCodeMasks::SetSoundTimer) == OpCodes::SetSoundTimer)
  {
    SetSoundTimer(nibbles[1]);
    return OpCodes::SetSoundTimer;
  }
  else if ((opcode & OpCodeMasks::AddToI) == OpCodes::AddToI)
  {
    AddToI(nibbles[1]);
    return OpCodes::AddToI;
  }
  else if ((opcode & OpCodeMasks::LoadFont) == OpCodes::LoadFont)
  {
    LoadFont(nibbles[1]);
    return OpCodes::LoadFont;
  }
  else if ((opcode & OpCodeMasks::LoadBigFont) == OpCodes::LoadBigFont)
  {
    LoadBigFont(nibbles[1]);
    return OpCodes::LoadBigFont;
  }
  else if ((opcode & OpCodeMasks::StoreBCD) == OpCodes::StoreBCD)
  {
    StoreBCD(nibbles[1]);
    return OpCodes::StoreBCD;
  }
  else if ((opcode & OpCodeMasks::StoreRegisters) == OpCodes::StoreRegisters)
  {
    StoreRegisters(nibbles[1]);
    return OpCodes::StoreRegisters;
  }
  else if ((opcode & OpCodeMasks::ReadRegisters) == OpCodes::ReadRegisters)
  {
    ReadRegisters(nibbles[1]);
    return OpCodes::ReadRegisters;
  }
  return OpCodes::Invalid;
}

const Emulator::byte_t Emulator::smallFont[16*5] = 
    {
      // 0
      0b11110000,
      0b10010000,
      0b10010000,
      0b10010000,
      0b11110000,
      // 1
      0b00100000,
      0b01100000,
      0b00100000,
      0b00100000,
      0b01110000,
      // 2
      0b11110000,
      0b00010000,
      0b11110000,
      0b10000000,
      0b11110000,
      // 3
      0b11110000,
      0b00010000,
      0b11110000,
      0b00010000,
      0b11110000,
      // 4
      0b10010000,
      0b10010000,
      0b11110000,
      0b00010000,
      0b00010000,
      // 5
      0b11110000,
      0b10000000,
      0b11110000,
      0b00010000,
      0b11110000,
      // 6
      0b11110000,
      0b10000000,
      0b11110000,
      0b10010000,
      0b11110000,
      // 7
      0b11110000,
      0b00010000,
      0b00100000,
      0b01000000,
      0b01000000,
      // 8
      0b11110000,
      0b10010000,
      0b11110000,
      0b10010000,
      0b11110000,
      // 9
      0b11110000,
      0b10010000,
      0b11110000,
      0b00010000,
      0b11110000,
      // A
      0b11110000,
      0b10010000,
      0b11110000,
      0b10010000,
      0b10010000,
      // B
      0b11100000,
      0b10010000,
      0b11100000,
      0b10010000,
      0b11100000,
      // C
      0b11110000,
      0b10000000,
      0b10000000,
      0b10000000,
      0b11110000,
      // D
      0b11100000,
      0b10010000,
      0b10010000,
      0b10010000,
      0b11100000,
      // E
      0b11110000,
      0b10000000,
      0b11110000,
      0b10000000,
      0b11110000,
      // F
      0b11110000,
      0b10000000,
      0b11110000,
      0b10000000,
      0b10000000
    };
}

