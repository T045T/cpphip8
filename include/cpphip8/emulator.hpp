// this is for emacs file handling -*- mode: c++; flycheck-clang-standard-library: "libc++"; flycheck-clang-language-standard: "c++11"; flycheck-clang-include-path: (".." "/usr/local/include/SDL2"); indent-tabs-mode: nil -*-

#ifndef CPPHIP8_EMULATOR
#define CPPHIP8_EMULATOR
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <random>
#include <array>

#include <iostream>
#include <iomanip>

#include "SDL.h"

#include "cpphip8/keyboard.hpp"
#include "cpphip8/screen.hpp"

namespace cpphip8
{
  class Emulator
  {
  public:
    typedef std::array<std::array<uint8_t, 64>, 128/8> screen_buffer_t;
    typedef uint16_t opcode_t;
    typedef uint16_t address_t;

    Emulator();
    void startEmulation();
    void stopEmulation();
    void decode(opcode_t opcode);
    void loadRom(std::string path);

    inline bool isRunning()
    {
      return alive;
    }

    inline unsigned int getXRes()
    {
      if (highres)
      {
        return 128;
      }
      else
      {
        return 64;
      }
    }

    inline unsigned int getYRes()
    {
      if (highres)
      {
        return 64;
      }
      else
      {
        return 32;
      }
    }

    // IO related stuff

    bool getPixel(unsigned int x, unsigned int y);
    inline screen_buffer_t getFramebuffer()
    {
      return frameBuffer;
    }

    /**
     * Was a screen update requested?
     * If so, return true and clear the screen update flag;
     */
    inline bool updateRequested()
    {
      if (screenUpdate)
      {
        std::lock_guard<std::mutex> lock(m);
        screenUpdate = false;
        return true;
      }
      else
      {
        return false;
      }
    }
    
    /**
     * Was a beep requested?
     * If so, return true and clear the screen update flag;
     */
    inline bool soundRequested()
    {
      if (soundRequest)
      {
        std::lock_guard<std::mutex> lock(m);
        soundRequest = false;
        return true;
      }
      else
      {
        return false;
      }
    }

    // Keyboard
    //! Notify the emulator of a pressed key
    void keyDown(uint8_t key);
    //! Notify the emulator of a released key
    void keyUp(uint8_t key);

    /**
     * I know, I KNOW. A nibble is supposed to have 4 bytes, but there's no uint4_t...
     */
    typedef uint8_t nibble_t;
    typedef uint8_t byte_t;

    class RegisterNames
    {
    public:
      enum Names
      {
        V0 = 0,
        V1,
        V2,
        V3,
        V4,
        V5,
        V6,
        V7,
        V8,
        V9,
        VA,
        VB,
        VC,
        VD,
        VE,
        VF
      };
    };

    // Parameters listed with length (byte = 8 bit, nibble = 4 bit, bnibble = 12 bit)
    class OpCodes
    {
    public:
      enum Codes
      {
        SysCode = 0x0000,
        ScrollDown = 0x00C0,
        ClearScreen = 0x00E0,
        Return = 0x00EE,
        ScrollRight = 0x00FB,
        ScrollLeft = 0x00FC,
        SetLowRes = 0x00FE,
        SetHighRes = 0x00FF,
        Jump = 0x1000,
        Call = 0x2000,
        SkipEqualImmediate = 0x3000,
        SkipNotEqualImmediate = 0x4000,
        SkipEqualRegister = 0x5000,
        LoadImmediate = 0x6000,
        AddToRegister = 0x7000,
        LoadRegister = 0x8000,
        OR = 0x8001,
        AND = 0x8002,
        XOR = 0x8003,
        ADD = 0x8004,
        SUB = 0x8005,
        ShiftRight = 0x8006,
        ReverseSUB = 0x8007,
        ShiftLeft = 0x800E,
        SkipNotEqualRegister = 0x9000,
        LoadI = 0xA000,
        JumpOffset = 0xB000,
        Random = 0xC000,
        DisplaySprite = 0xD000,
        SkipIfKey = 0xE09E,
        SkipIfNotKey = 0xE0A1,
        LoadDelayTimer = 0xF007,
        WaitForKey = 0xF00A,
        SetDelayTimer = 0xF015,
        SetSoundTimer = 0xF018,
        AddToI = 0xF01E,
        LoadFont = 0xF029,
        LoadBigFont = 0xF030,
        StoreBCD = 0xF033,
        StoreRegisters = 0xF055,
        ReadRegisters = 0xF065
      };
    };
    
    /**
     * These masks are used to ignore any parameter an OpCode takes
     * OpCodes are ANDed with the mask
     */
    class OpCodeMasks
    {
    public:
      enum Masks
      {
        SysCode = 0xF000,
        ScrollDown = 0xFFF0,
        ClearScreen = 0xFFFF,
        Return = 0xFFFF,
        ScrollRight = 0xFFFF,
        ScrollLeft = 0xFFFF,
        SetLowRes = 0xFFFF,
        SetHighRes = 0xFFFF,
        Jump = 0xF000,
        Call = 0xF000,
        SkipEqualImmediate = 0xF000,
        SkipNotEqualImmediate = 0xF000,
        SkipEqualRegister = 0xF00F,
        LoadImmediate = 0xF000,
        AddToRegister = 0xF000,
        LoadRegister = 0xF00F,
        OR = 0xF00F,
        AND = 0xF00F,
        XOR = 0xF00F,
        ADD = 0xF00F,
        SUB = 0xF00F,
        ShiftRight = 0xF0FF,
        ReverseSUB = 0xF00F,
        ShiftLeft = 0xF0FF,
        SkipNotEqualRegister = 0xF00F,
        LoadI = 0xF000,
        JumpOffset = 0xF000,
        Random = 0xF000,
        DisplaySprite = 0xF000,
        SkipIfKey = 0xF0FF,
        SkipIfNotKey = 0xF0FF,
        LoadDelayTimer = 0xF0FF,
        WaitForKey = 0xF0FF,
        SetDelayTimer = 0xF0FF,
        SetSoundTimer = 0xF0FF,
        AddToI = 0xF0FF,
        LoadFont = 0xF0FF,
        LoadBigFont = 0xF0FF,
        StoreBCD = 0xF0FF,
        StoreRegisters = 0xF0FF,
        ReadRegisters = 0xF0FF
      };
    };
  protected:
    static const uint8_t stack_size = 16;
    static const uint8_t FONT_BASE = 0;
    static const uint8_t LARGE_FONT_BASE = 80;
    static const address_t PROGRAM_START = 0x200;
    uint8_t memory[4096];
    uint8_t registers[17];
    uint8_t soundTimer;
    uint8_t delayTimer;
    uint16_t stack[stack_size];
    uint16_t INDEX;
    uint16_t PC;
    uint8_t SP;
    bool BEEP;
    bool highres;
    screen_buffer_t frameBuffer;
    std::default_random_engine random_generator;
    std::uniform_int_distribution<uint8_t> dist;

    // Thread stuff
    std::mutex m;
    std::condition_variable cv;
    std::thread t;
    bool alive;

    // Hardware stuff
    //! Set whenever a sprite is drawn or the screen is cleared
    bool screenUpdate;
    //! Set whenever the sound timer hits 0
    bool soundRequest;
    bool keys[16];

    void mainLoop();

    //! Copy the Font data into memory
    void setupFonts()
    {
      // 16 Characters * 5 lines
      for (size_t i = 0; i < 16*5; i++)
      {
        memory[i] = smallFont[i];
      }
    }

    opcode_t readOpcode(uint16_t address)
    {
      opcode_t ret = memory[address];
      ret = ret << 8;
      ret |= (memory[address+1]);
      return ret;
    }

    
    // Opcodes:
    
    /**
     * Execute native CPU code from address target
     * NOT gonna implement that shit. Nope. Nopenopenope.
     * @param target
     */
    void SysCode(address_t target)
    {}

    /**
     * scroll screen down x lines [SUPER]
     * @param x
     */
    void ScrollDown(nibble_t x);

    /**
     * Clear the screen. What did you think this would do? [SUPER]
     */
    inline void ClearScreen()
    {
      for (size_t x = 0; x < 128/8; x++)
      {
        for (size_t y = 0; y < 64; y++)
        {
          frameBuffer[x][y] = 0;
        }
      }
      std::lock_guard<std::mutex> lock(m);
      screenUpdate = true;
    }

    /**
     * Return from subroutine
     */
    inline void Return()
    {
      PC = stack[SP];
      stack[SP] = 0;
      if (SP > 0)
      {
        SP--;
      }
    }

    /**
     * Scroll screen right by 4 pixels [SUPER]
     */
    void ScrollRight();

    /**
     * Scroll screen left by 4 pixels [SUPER]
     */
    void ScrollLeft();

    /**
     * Set output to low resolution (64x32) [SUPER]
     */
    inline void SetLowRes()
    {
      highres = false;
      ClearScreen();
    }

    /**
     * Set output to high resolution (128x64) [SUPER]
     */
    inline void SetHighRes()
    {
      highres = true;
      ClearScreen();
    }

    /**
     * Jump to target address
     */
    inline void Jump(address_t target)
    {
      // Set PC to target - 2 because it's increased at the end of the main loop
      PC = target - 2;
    }

    /**
     * Jump to subroutine at target (push PC to stack)
     */
    inline void Call(address_t target)
    {
      if (SP < stack_size - 1)
      {
        SP += 1;
      }
      stack[SP] = PC;
      // Again, don't jump directly to target because PC is incremented in main loop
      PC = target - 2;
    }

    /**
     * Skip next instruction if registers[index] == constant
     */
    inline void SkipEqualImmediate(nibble_t index, byte_t constant)
    {
      //std::cout << "comparing " << (int) registers[index] << " to " << (int) constant << std::endl;
      if (registers[index] == constant)
      {
        //std::cout << "JUMP" << std::endl;
        PC+= 2;
      }
    }

    /**
     * Skip next instruction if registers[index] != constant
     */
    inline void SkipNotEqualImmediate(nibble_t index, byte_t constant)
    {
      if (registers[index] == constant)
      {
        PC+= 2;
      }
    }

    /**
     * Skip next instruction if registers[i] == registers[j]
     */
    inline void SkipEqualRegister(nibble_t i, nibble_t j)
    {
      if (registers[i] == registers[j])
      {
        PC+= 2;
      }
    }

    /**
     * Load constant into registers[index]
     */
    inline void LoadImmediate(nibble_t index, byte_t constant)
    {
      registers[index] = constant;
    }

    /**
     * Add constant to registers[index]
     */
    inline void AddToRegister(nibble_t index, byte_t constant)
    {
      registers[index] += constant;
    }

    /**
     * Load a value from one register into another
     */
    inline void LoadRegister(nibble_t target, nibble_t source)
    {
      registers[target] = registers[source];
    }

    //! OR two registers, writing into the first
    inline void OR(nibble_t target, nibble_t source)
    {
      registers[target] |= registers[source];
    }

    //! AND two registers, writing into the first
    inline void AND(nibble_t target, nibble_t source)
    {
      registers[target] &= registers[source];
    }

    //! XOR two registers, writing into the first
    inline void XOR(nibble_t target, nibble_t source)
    {
      registers[target] ^= registers[source];
    }

    //! registers[target] += registers[source] (carry ends up in VF)
    inline void ADD(nibble_t target, nibble_t source)
    {
      byte_t result = registers[target] + registers[source];
      if (result < registers[target])
      {
        registers[RegisterNames::VF] = 1;
      }
      registers[target] = result;
    }

    //! registers[target] -= registers[source] (borrow ends up in VF)
    inline void SUB(nibble_t target, nibble_t source)
    {
      byte_t result = registers[target] - registers[source];
      if (result > registers[target])
      {
        registers[RegisterNames::VF] = 1;
      }
      registers[target] = result;
    }

    //! Shift register right, rightmost bit goes to VF
    inline void ShiftRight(nibble_t index)
    {
      registers[RegisterNames::VF] = registers[index] & 1;
      registers[index] = registers[index] >> 1;
    }

    //! REVERSE SUBTRACT! registers[target] = registers[source] - registers[target], borrow in VF
    inline void ReverseSUB(nibble_t target, nibble_t source)
    {
      registers[target] = registers[source] - registers[target];
      if (registers[target] > registers[source])
      {
        registers[RegisterNames::VF] = 1;
      }
    }

    //! Shift register left, highest bit is shoved into VF
    inline void ShiftLeft(nibble_t index)
    {
      registers[RegisterNames::VF] = registers[index] & 0x80;
      registers[index] = registers[index] << 1;
    }

    //! Skip next instruction if registers[i] != registers[j]
    inline void SkipNotEqualRegister(nibble_t i, nibble_t j)
    {
      if (registers[i] != registers[j])
      {
        PC+= 2;
      }
    }

    //! Load Address from constant, into Index register (I)
    inline void LoadI(address_t address)
    {
      INDEX = address;
    }

    //! Jump to address + registers[0]
    inline void JumpOffset(address_t address)
    {
      Jump(address + registers[RegisterNames::V0]);
    }

    //! constant is ANDed with a random number <= 255 and the result written to registers[index]
    inline void Random(nibble_t index, byte_t constant)
    {
      byte_t rnd = dist(random_generator);
      registers[index] = constant & rnd;
    }

    /**
     * Display a sprite. 
     * Sprite base address is Index register (I).
     * Wraps around the screen, and is XORed into the frame buffer.
     * If that XOR clears any pixels, VF is set to 1
     * @param x index of the register holding the sprite's x coordinate
     * @param y index of the register holding the sprite's y coordinate
     * @param height height of the sprite - this many 8 bit lines of sprite are read. Special case 0: an "extended" sprite (16x16 bits) is read. This special case is [SUPER].
     */
    void DisplaySprite(nibble_t x, nibble_t y, nibble_t height);

    //! Skip next instruction if key with number equal to that stored in registers[key] is currently pressed
    inline void SkipIfKey(nibble_t index)
    {
      if (keys[registers[index]])
      {
        PC+= 2;
      }
    }

    //! Skip next instruction if key with number equal to that stored in registers[index] is currently NOT pressed
    inline void SkipIfNotKey(nibble_t index)
    {
      if (keys[registers[index]])
      {
        PC+= 2;
      }
    }

    //! Put current Delay value into registers[index]
    inline void LoadDelayTimer(nibble_t index)
    {
      registers[index] = delayTimer;
    }
    
    //! Halt until the key with number equal to registers[index] is pushed
    void WaitForKey(nibble_t index);
    
    //! Set Delay timer to value in registers[index]
    inline void SetDelayTimer(nibble_t index)
    {
      delayTimer = registers[index];
    }
    
    //! Set Sound timer to value in registers[index]
    void SetSoundTimer(nibble_t index)
    {
      soundTimer = registers[index];
    }
    
    //! Add value in registers[index] to I
    void AddToI(nibble_t index)
    {
      INDEX += registers[index];
    }

    //! Put the address of hex character registers[index] into I (5 lines high)
    void LoadFont(nibble_t index)
    {
      INDEX = FONT_BASE + registers[index] * 5;
    }
    
    //! Put the address of LARGE hex character registers[index] into I (10 lines high) [SUPER]
    void LoadBigFont(nibble_t index)
    {
      INDEX = LARGE_FONT_BASE + registers[index] * 10;
    }

    //! Store a BCD representation of registers[index] at I, I+1, I+2. Doesn't change I
    void StoreBCD(nibble_t index)
    {
      byte_t digits[3];
      digits[0] = registers[index] / 100;
      digits[1] = registers[index] % 100;
      digits[1] = digits[1] / 10;
      digits[2] = registers[index] % 10;
      for(size_t i = 0; i < 3; i++)
      {
        memory[INDEX+i] = digits[i];
      }
    }

    //! Store the registers up to registers[index] starting at I. Increments I past the last written byte
    void StoreRegisters(nibble_t index)
    {
      assert(index <= 0xF);
      for (size_t i = 0; i < index; i++)
      {
        memory[INDEX++] = registers[i];
      }
    }

    //! Load the registers up to registers[index] starting at I. Increments I past the last read byte
    void ReadRegisters(nibble_t index)
    {
      assert(index <= 0xF);
      for (size_t i = 0; i < index; i++)
      {
        registers[i] = memory[INDEX++];
      }
    }

    static const byte_t smallFont[16*5];

    static constexpr byte_t largeFont[16] =
    {
      0xDE,
      0xAD,
      0xBE,
      0xEF
    };
  };
}

#endif
