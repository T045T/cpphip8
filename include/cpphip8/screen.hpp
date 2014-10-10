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
  void refresh(uint8_t** framebuffer);
};
}

#endif
