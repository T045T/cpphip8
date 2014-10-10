#ifndef CPPHIP8_KEYBOARD
#define CPPHIP8_KEYBOARD
#include <cstdint>

namespace cpphip8
{
/**
 * Simulate a sixteen-key keyboard with a funky layout:
 * _________________
 * | 1 | 2 | 3 | C |
 * |---------------|
 * | 4 | 5 | 6 | D |
 * |---------------|
 * | 7 | 8 | 9 | E |
 * |---------------|
 * | A | 0 | B | F |
 * |---------------|
 */
class Keyboard
{
public:
  Keyboard();
  /**
   * Dummy implementation for now. Keys are never pressed. They cannot be pressed.
   * Because they don't exist
   */
  inline bool pollKey(uint8_t)
  {
    return false;
  }
};
}
#endif
