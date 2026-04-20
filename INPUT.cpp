#include "INPUT.h"
#include <algorithm>

void INPUT::handleINPUT(float dt)
{
	SDL_Event input;
	while (SDL_PollEvent(&input))
	{
		if (input.type == SDL_EVENT_QUIT)
		{
			RUN = false;
		}

		if (input.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (input.button.button == SDL_BUTTON_LEFT)
			{
				std::cout << "Left Click detetcted... do sum\n";
			}
		}
		if (input.type == SDL_EVENT_MOUSE_WHEEL)
		{
			C.zoom = std::clamp(C.zoom + (0.07 * input.wheel.y), 0.25, 9.0);
		}
	}

	const bool* keys = SDL_GetKeyboardState(nullptr);


	if (keys[SDL_SCANCODE_W]) C.y -= cameraSPEED * dt;
	if (keys[SDL_SCANCODE_S]) C.y += cameraSPEED * dt;
	if (keys[SDL_SCANCODE_A]) C.x -= cameraSPEED * dt;
	if (keys[SDL_SCANCODE_D]) C.x += cameraSPEED * dt;

}