// this is for emacs file handling -*- mode: c++; flycheck-clang-standard-library: "libc++"; flycheck-clang-language-standard: "c++11"; flycheck-clang-include-path: ("../include" "/usr/local/include/SDL2"); indent-tabs-mode: nil -*-

#include "cpphip8/emulator.hpp"
#include <thread>
#include <chrono>
#include "test.h"
#include "SDL.h"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    return 1;
  }

  if (!setupSDL())
  {
    return 1;
  }
  setupKeymap();

  cpphip8::Emulator emu;
  std::string romPath = argv[1];
  emu.loadRom(romPath);
  emu.startEmulation();

  bool okay = true;
  while(okay)
  {
    SDL_Event e;
    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
      switch(e.type)
      {
        //User requests quit
        case SDL_QUIT:
          okay = false;
          emu.stopEmulation();
          break;
        case SDL_KEYDOWN:
          // std::cout << "[Frontend] Key Down: " << (int) keyMap[e.key.keysym.sym] << std::endl;
          emu.keyDown(keyMap[e.key.keysym.sym]);
          break;
        case SDL_KEYUP:
          // std::cout << "[Frontend] Key Up: " << (int) keyMap[e.key.keysym.sym] << std::endl;
          emu.keyUp(keyMap[e.key.keysym.sym]);
          break;
      }
    }

    if (emu.updateRequested())
    {
      // Clear screen
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);

      // Draw screen
      SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

      int pixel_width = (xRes / (float) emu.getXRes());
      int pixel_height = (yRes / (float) emu.getYRes());

      SDL_Rect pixel = {0, 0, pixel_width, pixel_height};
      for (size_t x = 0; x < emu.getXRes(); x++)
      {
        for (size_t y = 0; y < emu.getXRes(); y++)
        {
          if (emu.getPixel(x, y))
          {
            pixel.x = x * pixel_width;
            pixel.y = y * pixel_height;
            SDL_RenderFillRect(renderer, &pixel);
          }
        }
      } 
      SDL_RenderPresent(renderer);
    }

    if (emu.soundRequested())
    {
      // Beep boop
    }

    if (!emu.isRunning())
    {
      okay = false;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
  }
  
  closeSDL();  
  return 0;
}

void setupKeymap()
{
  // Static for now
  keyMap.emplace(SDLK_1, 0x1);
  keyMap.emplace(SDLK_2, 0x2);
  keyMap.emplace(SDLK_3, 0x3);
  keyMap.emplace(SDLK_4, 0xC);

  keyMap.emplace(SDLK_q, 0x4);
  keyMap.emplace(SDLK_w, 0x5);
  keyMap.emplace(SDLK_e, 0x6);
  keyMap.emplace(SDLK_r, 0xD);

  keyMap.emplace(SDLK_a, 0x7);
  keyMap.emplace(SDLK_s, 0x8);
  keyMap.emplace(SDLK_d, 0x9);
  keyMap.emplace(SDLK_f, 0xE);

  keyMap.emplace(SDLK_y, 0xA);
  keyMap.emplace(SDLK_x, 0x0);
  keyMap.emplace(SDLK_c, 0xB);
  keyMap.emplace(SDLK_v, 0xF);
}

bool setupSDL()
{
  if ( SDL_Init (SDL_INIT_VIDEO) < 0)
  {
    std::cout << "Error initializing SDL!" << std::endl;
    return false;
  }

  window = SDL_CreateWindow( "CPPHIP8", 
                             SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                             xRes, yRes,
                             SDL_WINDOW_SHOWN);
  if (window == NULL)
  {
    std::cout << "Error initializing Window!" << std::endl;
    return false;
  }

  renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) 
  {
    std::cout << "Error initializing Renderer!" << std::endl;
  }
  return true;
}

void closeSDL()
{
  SDL_DestroyWindow(window);
  window = NULL;
  SDL_Quit();
}
