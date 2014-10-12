#ifndef CPPHIP8_OPCODES
#define CPPHIP8_OPCODES

namespace cpphip8
{
// Parameters listed with length (byte = 8 bit, nibble = 4 bit, bnibble = 12 bit)
class OpCodes
{
public:
  enum OpCode
  {
    Invalid = 0xFFFF,
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
  enum Mask
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
}

#endif
