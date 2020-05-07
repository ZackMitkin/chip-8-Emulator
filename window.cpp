#include "window.h"

void Window::create()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Create renderer for window
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}

			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);

			//Fill the surface white
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

			SDL_SetRenderTarget(renderer, NULL);

			//Update the surface
			SDL_UpdateWindowSurface(window);

			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(renderer);
		}
	}

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			SDL_Rect pixel = { x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE };
			pixels.push_back(pixel);
		}
	}
}

void Window::clear()
{
	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
}

void Window::render(std::array<bool, 64 * 32> pixelArray)
{
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			this->destroy();
		}
	}

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	for (int i = 0; i < pixelArray.size(); i++)
	{
		if (pixelArray[i] == 1)
		{
			SDL_RenderFillRect(renderer, &pixels[i]);
		}
	}

	//Update screen
	SDL_RenderPresent(renderer);
}

bool Window::getKeySate(bool key[])
{
	SDL_PumpEvents();

	//bool key[16] = { false };

	const Uint8* state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_1])      key[0x1] = true;
	else if (state[SDL_SCANCODE_2]) key[0x2] = true;
	else if (state[SDL_SCANCODE_3]) key[0x3] = true;
	else if (state[SDL_SCANCODE_4]) key[0xC] = true;

	else if (state[SDL_SCANCODE_Q]) key[0x4] = true;
	else if (state[SDL_SCANCODE_W]) key[0x5] = true;
	else if (state[SDL_SCANCODE_E]) key[0x6] = true;
	else if (state[SDL_SCANCODE_R]) key[0xD] = true;

	else if (state[SDL_SCANCODE_A]) key[0x7] = true;
	else if (state[SDL_SCANCODE_S]) key[0x8] = true;
	else if (state[SDL_SCANCODE_D]) key[0x9] = true;
	else if (state[SDL_SCANCODE_F]) key[0xE] = true;

	else if (state[SDL_SCANCODE_Z]) key[0xA] = true;
	else if (state[SDL_SCANCODE_X]) key[0x0] = true;
	else if (state[SDL_SCANCODE_C]) key[0xB] = true;
	else if (state[SDL_SCANCODE_V]) key[0xF] = true;
	else {
		return NULL;
	}
	return key;
}

void Window::destroy()
{
	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();
}