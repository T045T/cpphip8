#ifndef CPPHIP8_KEYBOARD
#define CPPHIP8_KEYBOARD
#include <cstdint>
#include <thread>
#include <mutex>

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
  ~Keyboard();
  
  /**
   * Returns true if the given key is currently pressed
   * @param key the key to query. Expected to be between 0x0 and 0xF
   */
  bool pollKey(uint8_t key);
protected:
  std::thread mythread;
  std::mutex m;
  void inputLoop();

  bool alive;
  bool keys[16];
  char keymap[16];
};
}
#endif
