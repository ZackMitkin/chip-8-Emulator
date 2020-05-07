#pragma once

#include <cstdio>
#include <array>
#include <SDL.h>

#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

const int PIXEL_SIZE = 10;

class Window
{
public:
	void create();
	void clear();
	void render(std::array<bool, 64 * 32>);
	void destroy();
	bool getKeySate(bool[]);
private:
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Renderer* renderer;
	SDL_Event e;

	std::vector <SDL_Rect> pixels; // how many pixels on the chip-8
};

