#ifndef TEST_EMU
#define TEST_EMU
#include "SDL.h"
#include <unordered_map>

bool setupSDL();
void setupKeymap();
void closeSDL();
size_t xRes = 800;
size_t yRes = 600;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Color on_color = {255, 255, 255, 255};
SDL_Color off_color = {0, 0, 0, 0};
std::unordered_map<unsigned int, unsigned int> keyMap;

#endif
