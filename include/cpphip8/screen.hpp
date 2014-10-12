#ifndef CPPHIP8_SCREEN
#define CPPHIP8_SCREEN

#include <cstdint>

namespace cpphip8
{
/**
 * This will do the drawing. Eventually. Hopefullly. Maybe.
 */
class Screen
{
public:
  Screen();
  void refresh(uint8_t (&framebuffer)[128/8][64]);
  inline uint8_t getXRes()
  {
    return currentXRes;
  }
  inline uint8_t getYRes()
  {
    return currentYRes;
  }
protected:
  uint8_t currentXRes;
  uint8_t currentYRes;
};
}

#endif
